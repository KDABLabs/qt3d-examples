# Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
#
# This file is part of the Qt3D examples module.
#
# GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser
# General Public License version 3 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL3 included in the
# packaging of this file. Please review the following information to
# ensure the GNU Lesser General Public License version 3 requirements
# will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 2.0 or (at your option) the GNU General
# Public license version 3 or any later version approved by KDAB.
# The licenses are as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
# included in the packaging of this file. Please review the following
# information to ensure the GNU General Public License requirements will
# be met: https://www.gnu.org/licenses/gpl-2.0.html and
# https://www.gnu.org/licenses/gpl-3.0.html.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
#

import bpy
import os
import mathutils
import math
from .material import MaterialCollection
from .mesh import MeshCollection, Mesh, SubMesh
from .node import Node
from .utils import propertyName, qmlFileName, localMatrix4x4FromBObject, matrix4x4FromBMatrix, blenderColorToQColor, blender3DVectorToQVector3D

class Exporter(object):

    def __init__(self, scene, settings):
        self.scene = scene
        self.settings = settings
        self.qmlGenerators = {
            "MESH": self.generateQMLFileForMesh,
            "CAMERA": self.generateQMLFileForCamera,
            "LAMP": self.generateQMLFileForLamp,
        }

    def export(self):
        self.materialCollectionInfo = ({"elementName": "MaterialCollection",
                                        "fileName": "MaterialCollection.qml",
                                        "id": "_materialCollection",
                                        "fileContent": ""})
        self.meshCollectionInfo = ({"elementName": "MeshCollection",
                                    "fileName": "MeshCollection.qml",
                                    "id": "_meshCollection",
                                    "fileContent": ""})
        self.qmlDirectoryName = "qml"

        # create Material collection
        self.materialCollection = MaterialCollection()

        # create Mesh collection
        # Generate a GeometryRenderer for each Blender Mesh
        # Note: since a Blender Mesh can have multiple materials for different faces
        # This result in 1 GeometryRenderer per Material
        # So 1 Blender Mesh may endup into several Qt3D GeometryRenderers
        self.meshCollection = MeshCollection(applyModifiers=self.settings["use_mesh_modifiers"])

        self.qmlResourceFiles = []
        self.binaryResourceFiles = self.meshCollection.resources
        self.shaderResourceFiles = self.materialCollection.resources


    def createQMLFileForNode(self, root):
        for node in root.children:
            self.createQMLFileForNode(node)

        isMain = root.bObject is None

        if not self.settings["export_full_qt3d_app"] and isMain:
            return

        qmlFile = "main.qml" if isMain else qmlFileName(root.bObject.name) + ".qml"

        content = ("import Qt3D.Core 2.9\n"
                   "import Qt3D.Render 2.9\n"
                   "import Qt3D.Extras 2.9\n" # Needed for the default material in case none was specified (this is a minority of cases actually)
                   "" + ("import Qt3D.Input 2.1\n" if isMain else "") + "\n")

        if isMain:
            content += self.generateMainQMLFile(root)
        else:
            print("Type " + str(root.bObject.type))
            if root.bObject.type in self.qmlGenerators:
                content += self.qmlGenerators[root.bObject.type](root)
            else: # Grouping Object
                content += ("Entity {\n"
                            "    components: [ Transform { matrix: " + localMatrix4x4FromBObject(root.bObject) + "} ]\n\n")


        for node in root.children:
            childElementName = qmlFileName(node.bObject.name)
            elementIdName = propertyName(node.bObject.name)
            offset = 8 if isMain else 4
            content += (" " * offset + "" +
                        childElementName + " {\n" +
                        " " * offset +
                        "    id: " + elementIdName + "\n" +
                        " " * offset +
                        "}\n\n")

        # Close Brace for main transform Entity
        if isMain:
            content += "    }\n"
        content += "}\n"

        self.qmlResourceFiles.append(os.path.join(self.qmlDirectoryName, qmlFile))
        with open(self.qmlResourceFiles[-1], "w") as f:
            f.write(content)


    def createQMLNodeTree(self):
        rootNode = Node()
        nodes = {}

        sceneObjects = self.scene.objects
        if self.settings["use_selection_only"]:
            sceneObjects = [obj for obj in sceneObjects if obj.select]
        if self.settings["use_visible_only"]:
            sceneObjects = [obj for obj in sceneObjects if not obj.hide]

        for obj in sceneObjects:
            nodes[obj.name] = Node(obj)

        for nEntry in nodes.items():
            node = nEntry[1]
            if not node.parentName():
                rootNode.addChild(node)
            else:
                nodes[node.parentName()].addChild(node)

        self.createQMLFileForNode(rootNode)

        if self.settings["use_material_collection"]:
            self.materialCollectionInfo["fileContent"] = str(self.materialCollection)
            self.qmlResourceFiles.append(os.path.join(self.qmlDirectoryName, self.materialCollectionInfo["fileName"]))
            with open(self.qmlResourceFiles[-1], "w") as f:
                f.write(self.materialCollectionInfo["fileContent"])

        if self.settings["use_mesh_collection"]:
            self.meshCollectionInfo["fileContent"] = str(self.meshCollection)
            self.qmlResourceFiles.append(os.path.join(self.qmlDirectoryName, self.meshCollectionInfo["fileName"]))
            with open(self.qmlResourceFiles[-1], "w") as f:
                f.write(self.meshCollectionInfo["fileContent"])


    def generateQMLFileForMesh(self, node):
        content = ("Entity {\n"
                   "    id: root\n\n")

        # Create Mesh + SubMeshes file
        # Note: to access the mesh datablock we need to do object.data
        print("Object MESH " + node.bObject.data.name)

        # Retrieve Mesh object from name
        mesh = self.meshCollection.meshForName(node.bObject.data.name)
        if mesh is None:
            raise Exception("No mesh found for blenderMesh " + node.bObject.data.name)

        subMeshes = mesh.subMeshes
        nestedMeshes = len(subMeshes) > 1
        meshPropertyName = mesh.meshPropertyName

        if not self.settings["use_material_collection"]:
            for subMesh in subMeshes:
                print("Material name for submesh " + subMesh.materialName)
                material = self.materialCollection.materialForPropertyName(subMesh.materialName)
                if material is not None:
                    content += str(material)

        # Instantiate buffers and attributes if not using a mesh collection
        if not self.settings["use_mesh_collection"]:
            content += str(mesh)

        for subMesh in subMeshes:
            materialName = subMesh.materialName
            print("Material Name for Submesh is: " + materialName)
            offset = 4 if nestedMeshes else 0
            if nestedMeshes:
                content += "   Entity {\n"

            qt3dMaterialProperty = (((self.materialCollectionInfo["id"] + ".") if self.settings["use_material_collection"] else "") + materialName) if len(materialName) > 0 else "PhongMaterial {}"

            content += (" " * offset +
                        "    readonly property Material material: " + qt3dMaterialProperty + "\n" +
                        " " * offset +
                        "    readonly property Transform transform: Transform { matrix: " + localMatrix4x4FromBObject(node.bObject) + "}\n" +
                        " " * offset +
                        "    readonly property GeometryRenderer geometryRenderer: " + ((self.meshCollectionInfo["id"] + ".") if self.settings["use_mesh_collection"] else "") + meshPropertyName + materialName + "\n" +
                        " " * offset +
                        "    components: [transform, material, geometryRenderer]\n")
            if nestedMeshes:
                content += " " * offset + "}\n\n"

        return content

    def generateQMLFileForLamp(self, node):
        lamp = node.bObject.data
        print("Lamp " + str(lamp) + " " + str(lamp.type))

        lampComponentNameFromType = {"POINT": "PointLight",
                                     "SUN": "DirectionalLight",
                                     "SPOT": "SpotLight", }
        if str(lamp.type) in lampComponentNameFromType:
            print("Generating Lamp")
            content = ("Entity {\n"
                       "    id: root\n\n"
                       "    components: [\n"
                       "        Transform {\n" +
                       "            matrix: " + localMatrix4x4FromBObject(node.bObject) + "\n"
                       "        },\n"
                       "        " + lampComponentNameFromType[lamp.type] + "{\n"
                       "            color:" + blenderColorToQColor(lamp.color) + "\n"
                       "            intensity: " + str(lamp.energy) + "\n")

            if lamp.type == "SPOT":
                content += ("            cutOffAngle: " + str(math.degrees(lamp.spot_size)) + "\n"
                            "            constantAttenuation: " + str(lamp.constant_coefficient) + "\n"
                            "            linearAttenuation: " + str(lamp.linear_attenuation) + "\n"
                            "            quadraticAttenuation: " + str(lamp.quadratic_attenuation) + "\n"
                            "            localDirection: " + blender3DVectorToQVector3D(mathutils.Vector(0, 0, -1) * node.bObject.matrix_local) + "\n")
            elif lamp.type == "POINT":
                content += ("            constantAttenuation: " + str(lamp.constant_coefficient) + "\n"
                            "            linearAttenuation: " + str(lamp.linear_attenuation) + "\n"
                            "            quadraticAttenuation: " + str(lamp.quadratic_attenuation) + "\n")
            elif lamp.type == "SUN":
                content += ("            worldDirection: " + blender3DVectorToQVector3D(mathutils.Vector(0, 0, -1) * node.bObject.matrix_world) + "\n")

            content += ("        }\n"
                        "    ]\n\n")
            return content
        return ""

    def generateMainQMLFile(self, node):
        # FrameGraph and Render/Input Settings
        content = ("Entity {\n"
                   "    id: mainRoot\n\n"
                   "    components: [\n"
                   "       RenderSettings {\n"
                   "           activeFrameGraph: ForwardRenderer {\n"
                   "               clearColor: Qt.rgba(0, 0.5, 1, 1)\n"
                   "               camera: " + propertyName(self.scene.camera.name) + "\n"
                   "               frustumCulling: false\n"
                   "           }\n"
                   "       },\n"
                   "       // Event Source will be set by the Qt3DQuickWindow\n"
                   "       InputSettings { }\n"
                   "    ]\n\n"
                   "    OrbitCameraController\n"
                   "    {\n"
                   "        camera: camera\n"
                   "    }\n\n")

        # Instantiate Mesh and Material collections if required
        collections = []
        if self.settings["use_mesh_collection"]:
            collections.append(self.meshCollectionInfo)
        if self.settings["use_material_collection"]:
            collections.append(self.materialCollectionInfo)
        for collection in collections:
            content += ("    " + collection["elementName"] + " {\n"
                        "        id: " + collection["id"] + "\n"
                        "    }\n\n")

        content += ("    Entity {\n"
                    "        // Apply global blender transformation\n"
                    "        components: Transform { matrix: " + matrix4x4FromBMatrix(self.settings["global_matrix"]) + "}\n\n")
        return content

    def generateQMLFileForCamera(self, node):
        # Create Camera File
        # TO DO: Complete CameraLens and Transform
        # Note: use z as up when defining camera as global transform will
        # scene transform will transform to Y up if required
        camera = node.bObject.data
        location = node.bObject.matrix_local.to_translation()
        # direction = node.bObject.matrix_local.to_quaternion() * mathutils.Vector((0.0, 0.0, -1.0))
        up = node.bObject.matrix_local.to_quaternion() * mathutils.Vector((0.0, 1.0, 0.0))
        aspectRatio = (self.scene.render.resolution_x * self.scene.render.pixel_aspect_x) / (self.scene.render.resolution_y * self.scene.render.pixel_aspect_y)

        projectionTypes = {
            "PERSP": "CameraLens.PerspectiveProjection",
            "ORTHO": "CameraLens.OrthographicProjection",
            "PANO": "CameraLens.PerspectiveProjection"
        }

        content = ("import Qt3D.Extras 2.9\n\n"
                   "Camera {\n"
                   "    id: camera\n"
                   "    aspectRatio: " + str(aspectRatio) + "\n"
                   "    nearPlane: " + str(camera.clip_start) + "\n"
                   "    farPlane: " + str(camera.clip_end) + "\n"
                   "    projectionType: " + projectionTypes[camera.type] + "\n"
                   "    fieldOfView: " + str(math.degrees(camera.angle_y)) + "\n"
                   "    position: " + blender3DVectorToQVector3D(location) + "\n"
                   "    upVector: " + blender3DVectorToQVector3D(up) + "\n"
                   "    viewCenter: Qt.vector3d(0, 0, 0) \n")

        return content

    def generateQRCFiles(self):
        self.generateQRCFile(self.qmlResourceFiles, "qml.qrc")
        self.generateQRCFile(self.binaryResourceFiles, "buffers.qrc")
        self.generateQRCFile(self.shaderResourceFiles, "shaders.qrc")

    def generateQRCFile(self, resources, fileName):
        content = ("<RCC>\n"
                   "    <qresource prefix=\"/\">\n")
        for resource in resources:
            content += (8 * " " + "<file>%s</file>\n" % resource)
        content += ("   </qresource>\n"
                    "</RCC>\n")

        with open(fileName, "w") as f:
            f.write(content)


    def generateMainCppFile(self):
        content = ("#include <Qt3DQuickExtras/qt3dquickwindow.h>\n"
                   "#include <QGuiApplication>\n"
                   "#include <QResource>\n"
                   "int main(int argc, char* argv[])\n"
                   "{\n"
                   "    QGuiApplication app(argc, argv);\n"
                   "    Qt3DExtras::Quick::Qt3DQuickWindow view;\n"
                   "    QResource::registerResource(\"buffers.qrb\");\n"
                   "    QResource::registerResource(\"shaders.qrb\");\n"
                   "    view.setSource(QUrl(\"qrc:/qml/main.qml\"));\n"
                   "    view.setWidth(" + str(self.scene.render.resolution_x * self.scene.render.pixel_aspect_x) + ");\n"
                   "    view.setHeight(" + str(self.scene.render.resolution_y * self.scene.render.pixel_aspect_y) + ");\n"
                   "    view.show();\n"
                   "    return app.exec();\n"
                   "}\n")

        with open("main.cpp", "w") as f:
            f.write(content)


    def generateProjectFile(self, userpath):
        content = ("QT += 3dcore 3drender 3dinput 3dquick 3dlogic qml quick 3dquickextras\n\n"
                   "SOURCES += \\\n"
                   "    main.cpp\n\n"
                   "RESOURCES += \\\n"
                   "    qml.qrc\n\n"
                   "RCC_BINARY_SOURCES += \\\n"
                   "         $$PWD/buffers.qrc \\\n"
                   "         $$PWD/shaders.qrc\n\n"
                   "asset_builder.commands = $$[QT_HOST_BINS]/rcc -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} -no-compress\n"
                   "asset_builder.depend_command = $$[QT_HOST_BINS]/rcc -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}\n"
                   "asset_builder.input = RCC_BINARY_SOURCES\n"
                   "asset_builder.output = $$OUT_PWD/${QMAKE_FILE_IN_BASE}.qrb\n"
                   "asset_builder.CONFIG += no_link target_predeps\n"
                   "QMAKE_EXTRA_COMPILERS += asset_builder\n")
        print("Split " + str(os.path.split(userpath)))
        dirs = os.path.split(userpath)
        dirname = (dirs[-2].lower() if len(dirs[-1]) == 0 else dirs[-1].lower()) + ".pro"
        print("Path " + userpath + " - " + dirname)
        with open(dirname, "w") as f:
            f.write(content)




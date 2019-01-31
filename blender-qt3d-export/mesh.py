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
from array import array
from .utils import propertyName, cleanString

class MeshCollection(object):
    def __init__(self, applyModifiers=True):
        # Filter out meshes, apply modifiers...

        # Retrieve all modifiers object
        modifiersObject = [obj for obj in bpy.data.objects if [modifier for modifier in obj.modifiers if modifier.type != "ARMATURE"]]

        # Retrieve all meshes, apply modifiers on meshes that have modifiers
        blenderMeshes = {}

        # Copy list of meshes (as applying modifiers might add entries in this list)
        originalMeshes = [mesh for mesh in bpy.data.meshes]
        if applyModifiers:
            for blenderMesh in originalMeshes:
                modifiersForMesh = [obj for obj in modifiersObject if obj.data == blenderMesh]

                if len(modifiersForMesh) > 0:
                    # We have modifiers
                    modifiedMesh = blenderMesh
                    # Apply all modifiers
                    for modifier in modifiersForMesh:
                        modifiedMesh = modifier.to_mesh(bpy.context.scene, True, "PREVIEW")
                        modifiersForMesh = [obj for obj in modifiersObject if obj.data == modifiedMesh]
                    # Save origin mesh name and reference modified mesh
                    blenderMeshes[blenderMesh.name] = modifiedMesh
                else:
                    # Mesh has no modifiers
                    blenderMeshes[blenderMesh.name] = blenderMesh
        else:
            blenderMeshes = {blenderMesh.name : blenderMesh for blenderMesh in originalMeshes}

        self.meshes = [Mesh(blenderMeshName, blenderMesh) for blenderMeshName, blenderMesh in blenderMeshes.items()]
        self.meshCollectionContent = ("import Qt3D.Core 2.9\n"
                                      "import Qt3D.Render 2.9\n"
                                      "\n"
                                      "Entity {\n"
                                      "    id: root\n\n"
                                      "%s\n"
                                      "\n"
                                      "}\n") % "\n".join([str(mesh) for mesh in self.meshes])

    def __str__(self):
        return self.meshCollectionContent

    @property
    def content(self):
        return self.meshCollectionContent

    @property
    def resources(self):
        return [resource for mesh in self.meshes for resource in mesh.resources]

    def meshForName(self, blenderMeshName):
        for mesh in self.meshes:
            if mesh.blenderMeshName == blenderMeshName:
                return mesh
        return None

class Mesh(object):
    def __init__(self, meshName, mesh):
        self.meshName = meshName
        self.blenderMesh = mesh
        self._meshPropertyName = propertyName(cleanString(self.meshName))
        self.resourceFiles = []
        self.binaryDirectoryName = "assets/binaries/"
        # A Mesh contains an array of vertices and a set of faces
        # Each face contains:
        # * indices of vertex into the mesh vertices array
        # * index of Material to be used for the mesh

        print("Creating GeometryRenderers for ",  self.blenderMesh.name)

        # Convert Mesh to triangulated faces and build buffer

        # Triangulate blenderMesh
        self.blenderMesh.calc_normals_split()
        self.blenderMesh.calc_tessface()

        # Retrieve vertices from mesh (use dictionary hashing to remove duplicates)
        self.vertices = {Vertex(mesh, loop): 0 for loop in self.blenderMesh.loops}.keys()

        self.vertexBufferName = "vertexBuffer" + self.meshPropertyName
        self.indexBufferName = "indexBuffer" + self.meshPropertyName
        self.verticesFileName = os.path.join(self.binaryDirectoryName, self.meshPropertyName + ".vertices")
        self.indicesFileName = os.path.join(self.binaryDirectoryName, self.meshPropertyName + ".indices")

        self.buffers = [Buffer(self.vertexBufferName, self.verticesFileName), Buffer(self.indexBufferName, self.indicesFileName)]

        # Generate Vertex Attributes
        self.vertexData = []
        self.vertexAttributes = []
        self.prepareVertexData()

        # Generate SubMeshes
        self.usesUnsignedShortIndexing = True
        self._subMeshes = []
        self.indexData = []
        self.prepareIndexData()

        self.generateContent()


    @property
    def blenderMeshName(self):
        return self.meshName

    @property
    def meshPropertyName(self):
        return self._meshPropertyName

    @property
    def subMeshes(self):
        return self._subMeshes

    def prepareVertexData(self):
        num_uvs = len(self.blenderMesh.uv_layers)
        num_cols = len(self.blenderMesh.vertex_colors)
        # position (3), normal (3), uv (2), color (3)
        # in bytes
        vertexByteSize = (3 + 3 + num_uvs * 2 + num_cols * 3) * 4
        requiredAttributes = [AttributeType.POSITION, AttributeType.NORMAL] + [AttributeType.TEXT_COORD0 + i for i in range(0, num_uvs)] + [AttributeType.COLOR0 + i for i in range(0, num_cols)]

        byteOffset = 0;
        for attributeInfo in requiredAttributes:
            self.vertexAttributes.append(VertexAttribute(self.meshPropertyName, self.vertexBufferName, vertexByteSize, byteOffset, attributeInfo))
            byteOffset += VertexAttribute.vertexSizeFromType[attributeInfo] * 4

        # Convert each vertex to base list of floats
        for idx, vertex in enumerate(self.vertices):
            # Save index for later
            vertex.index = idx
            vertexData = [vertex.co[0], vertex.co[1], vertex.co[2], vertex.normal[0], vertex.normal[1], vertex.normal[2]]
            for j, uv in enumerate(vertex.uvs):
                vertexData += [uv[0], uv[1]]
            for j, col in enumerate(vertex.colors):
                vertexData += [col[0], col[1], col[2]]
            self.vertexData.extend(vertexData)

    def prepareIndexData(self):
        # Retrieve materials for mesh (allows us to know how many geometry renderers we need)
        meshMaterials = [material for material in self.blenderMesh.materials]
        # Create a dictionary { materialName: meshIndices }
        subMeshesPerMaterial = {ma.name: [] for ma in meshMaterials}
        if not subMeshesPerMaterial:
            subMeshesPerMaterial = {"": []}

        # Create mapping between loop indices and vertices
        vertexIndices = {i: vertex for vertex in self.vertices for i in vertex.loop_indices}
        maxIdxCount = 0
        for poly in self.blenderMesh.polygons:
            # Try to retrieve material for poly (though none could be set)
            if not meshMaterials:
                subMeshForMaterial = subMeshesPerMaterial[""]
            else:
                try:
                    material = meshMaterials[poly.material_index]
                except IndexError:
                    continue
                # Retrieve subMesh for the given material
                subMeshForMaterial = subMeshesPerMaterial[material.name if material else ""]
            # Retrieve indices for the polygon
            indices = [vertexIndices[i].index for i in poly.loop_indices]
            # Update max index value
            maxIdxCount = max(maxIdxCount, max(indices))
            indexCount = len(indices)
            if indexCount == 3:
                # Perfect poly is a triangle
                subMeshForMaterial += indices
            elif indexCount > 3:
                # We need to triangulate
                for i in range(indexCount - 2):
                    subMeshForMaterial += [indices[-1], indices[i], indices[i + 1]]
            else:
                # We got something that cannot be converted to a triangle
                raise RuntimeError("Invalid polygon with less than 3 vertices")

        self.usesUnsignedShortIndexing = maxIdxCount <= 65535
        indexAttributeType = IndexAttributeType.SHORT if self.usesUnsignedShortIndexing else IndexAttributeType.FULL

        # We now have a list of subMeshes per material
        indexStartOffset = 0
        for material, subMeshes in subMeshesPerMaterial.items():
            # Skip if we have no subMeshes for a given material
            if not subMeshes:
                continue

            # Concatenate all the submeshes with the same material into the same contiguous part of the index buffer
            indexAttribute = IndexAttribute(indexAttributeType, self.indexBufferName, indexStartOffset, len(subMeshes))
            self._subMeshes.append(SubMesh(self.meshPropertyName, cleanString(propertyName(material)), indexAttribute, self.vertexAttributes))
            indexStartOffset += len(subMeshes)
            self.indexData.extend(subMeshes)


    def generateContent(self):
        # Vertex Buffer
        self.resourceFiles.append(self.verticesFileName)
        # Write array of floats to file
        with open(self.resourceFiles[-1], "wb") as f:
            floatArray = array("f", self.vertexData)
            floatArray.tofile(f)

        # Index Buffer
        self.resourceFiles.append(self.indicesFileName)
        # Write Index Buffer to file
        with open(self.resourceFiles[-1], "wb") as f:
            indicesArray = array("H" if self.usesUnsignedShortIndexing else "I", self.indexData)
            indicesArray.tofile(f)

        # Generate QML content for buffers and attributes
        self._content = "\n".join([str(buffer) for buffer in self.buffers]) + "\n".join([str(attribute) for attribute in self.vertexAttributes]) + "\n".join([str(subMesh) for subMesh in self._subMeshes])


    @property
    def content(self):
        return self._content

    @property
    def resources(self):
        return self.resourceFiles

    def __str__(self):
        return self.content


class Vertex:
    def __init__(self, blenderMesh, blenderLoop):
        loopIdx = blenderLoop.index
        vertexIdx = blenderLoop.vertex_index
        self.index = 0 # Will be set when building vertex buffer
        self.co = blenderMesh.vertices[vertexIdx].co[:]
        self.normal = blenderLoop.normal[:]
        self.uvs = tuple(layer.data[loopIdx].uv[:] for layer in blenderMesh.uv_layers)
        self.colors = tuple(layer.data[loopIdx].color[:] for layer in blenderMesh.vertex_colors)
        self.loop_indices = [loopIdx]

class Buffer(object):
    def __init__(self, vertexBufferName, verticesFileName):
        self._content = ("    Buffer {\n"
                         "        id: " + vertexBufferName + "\n"
                         "        type: Buffer.VertexBuffer\n"
                         "        data: readBinaryFile(\"qrc:/" + verticesFileName + "\")\n"
                         "    }\n")

    @property
    def content(self):
        return self._content

    def __str__(self):
        return self._content


class AttributeType:
    POSITION = 0
    NORMAL = 1
    TEXT_COORD0 = 2
    TEXT_COORD1 = 3
    TEXT_COORD2 = 4
    COLOR0 = 5
    COLOR1 = 6
    COLOR2 = 7

class IndexAttributeType:
    SHORT = 0,
    FULL = 1

class IndexAttribute(object):
    def __init__(self, indexAttributeType, indexBufferName, indexOffset, indexCount):
        usesUnsignedShortIndexing = indexAttributeType == IndexAttributeType.SHORT
        self._content = ("             readonly property Attribute indexAttribute: Attribute {\n"
                         "                 attributeType: Attribute.IndexAttribute\n"
                         "                 vertexBaseType: " + ("Attribute.UnsignedShort" if usesUnsignedShortIndexing else "Attribute.UnsignedInt") + "\n"
                         "                 vertexSize: 1\n"
                         "                 byteOffset: %d * " + ("2" if usesUnsignedShortIndexing else "4") + "\n"
                         "                 byteStride: " + ("2" if usesUnsignedShortIndexing else "4") + "\n"
                         "                 count: %d\n"
                         "                 buffer: " + indexBufferName + "\n"
                         "               }\n") % (indexOffset, indexCount)

    @property
    def content(self):
        return self._content

    def __str__(self):
        return self._content

class VertexAttribute(object):

    attributeNamesFromType = {
        AttributeType.POSITION: "defaultPositionAttributeName",
        AttributeType.NORMAL: "defaultNormalAttributeName",
        AttributeType.TEXT_COORD0: "defaultTextureCoordinateAttributeName",
        AttributeType.TEXT_COORD1: "defaultTextureCoordinate1AttributeName",
        AttributeType.TEXT_COORD2: "defaultTextureCoordinate2AttributeName",
        AttributeType.COLOR0: "defaultColorAttributeName",
        AttributeType.COLOR1: "defaultColor1AttributeName",
        AttributeType.COLOR2: "defaultColor2AttributeName"
    }

    vertexSizeFromType = {
        AttributeType.POSITION: 3,
        AttributeType.NORMAL: 3,
        AttributeType.TEXT_COORD0: 2,
        AttributeType.TEXT_COORD1: 2,
        AttributeType.TEXT_COORD2: 2,
        AttributeType.COLOR0: 3,
        AttributeType.COLOR1: 3,
        AttributeType.COLOR2: 3
    }

    attributeTypeName = {
        AttributeType.POSITION: "Position",
        AttributeType.NORMAL: "Normal",
        AttributeType.TEXT_COORD0: "TexCoord0",
        AttributeType.TEXT_COORD1: "TexCoord1",
        AttributeType.TEXT_COORD2: "TexCoord2",
        AttributeType.COLOR0: "Color0",
        AttributeType.COLOR1: "Color1",
        AttributeType.COLOR2: "Color2"
    }

    def __init__(self, meshPropertyName, vertexBufferName, stride, offset, attributeType):
        self._attributeIdName = meshPropertyName + VertexAttribute.attributeTypeName[attributeType] + "VertexAttribute"
        self._content = ("    Attribute {\n"
                         "        id: " +  self._attributeIdName + "\n"
                         "        attributeType: Attribute.VertexAttribute\n"
                         "        vertexBaseType: Attribute.Float\n"
                         "        vertexSize: " + str(VertexAttribute.vertexSizeFromType[attributeType]) + "\n"
                         "        byteOffset: " + str(offset) + "\n"
                         "        byteStride: " + str(stride) + "\n"
                         "        name: " + VertexAttribute.attributeNamesFromType[attributeType] + "\n"
                         "        buffer: " + vertexBufferName + "\n"
                         "    }\n")

    @property
    def attributeIdName(self):
        return self._attributeIdName

    @property
    def content(self):
        return self._content

    def __str__(self):
        return self._content

class SubMesh(object):
    def __init__(self, meshPropertyName, materialName, indexAttribute, vertexAttributes):
        self._materialName = materialName
        self._content = ("    readonly property GeometryRenderer " + meshPropertyName + materialName + ": GeometryRenderer {\n"
                         "        instanceCount: 1\n"
                         "        indexOffset: 0\n"
                         "        firstInstance: 0\n"
                         "        primitiveType: GeometryRenderer.Triangles\n"
                         "        geometry: Geometry {\n"
                         "" + (str(indexAttribute)) + "\n"
                         "            attributes: [indexAttribute, " + ", ".join([attribute.attributeIdName for attribute in vertexAttributes]) + "]\n"
                         "        }\n"
                         "    }\n"
                         "\n");

    @property
    def content(self):
        return self._content

    @property
    def materialName(self):
        return self._materialName

    def __str__(self):
        return self._content

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
import gpu
import os
from .utils import propertyName, blenderColorToQColor


def uniformInt1(value):
    pass


def uniformFloat1(value):
    pass


def uniformFloat2(value):
    pass


def uniformFloat3(value):
    pass


def uniformFloat4(value):
    pass


def uniformMat3(value):
    pass


def uniformMat4(value):
    pass


def uniformBool4(value):
    pass


def uniformDynamicObjViewMat(uniform):
    pass


def uniformDynamicObjMat(uniform):
    pass


def uniformDynamicObjViewIMat(uniform):
    pass


def uniformDynamicObjIMat(uniform):
    pass


def uniformDynamicObjectColor(uniform):
    pass


def uniformDynamicLampDynVec(uniform):
    pass


def uniformDynamicLampDynCo(uniform):
    pass


def uniformDynamicLampDynIMat(uniform):
    pass


def uniformDynamicLampPersMat(uniform):
    pass


def uniformDynamicLampDynEnergy(uniform):
    pass


def uniformDynamicLampDynCol(uniform):
    pass


def uniformDynamicSampler2DBuffer(uniform):
    pass


def uniformDynamicSampler2DImage(uniform):
    pass


def uniformDynamicSampler2DShadow(uniform):
    pass


def uniformDynamicObjectAutoBumpScale(uniform):
    pass


def parameterUniformValue(uniform):
    # Uniform is either a texture or a lamp property
    uniform_data_type = {
        gpu.GPU_DATA_1I: uniformInt1,
        gpu.GPU_DATA_1F: uniformFloat1,
        gpu.GPU_DATA_2F: uniformFloat2,
        gpu.GPU_DATA_3F: uniformFloat3,
        gpu.GPU_DATA_4F: uniformFloat4,
        gpu.GPU_DATA_9F: uniformMat3,
        gpu.GPU_DATA_16F: uniformMat4,
        gpu.GPU_DATA_4UB: uniformBool4
    }

    uniform_types = {
        gpu.GPU_DYNAMIC_OBJECT_VIEWMAT: uniformDynamicObjViewMat,
        gpu.GPU_DYNAMIC_OBJECT_MAT: uniformDynamicObjMat,
        gpu.GPU_DYNAMIC_OBJECT_VIEWIMAT: uniformDynamicObjViewIMat,
        gpu.GPU_DYNAMIC_OBJECT_IMAT: uniformDynamicObjIMat,
        gpu.GPU_DYNAMIC_OBJECT_COLOR: uniformDynamicObjectColor,
        gpu.GPU_DYNAMIC_LAMP_DYNVEC: uniformDynamicLampDynVec,
        gpu.GPU_DYNAMIC_LAMP_DYNCO: uniformDynamicLampDynCo,
        gpu.GPU_DYNAMIC_LAMP_DYNIMAT: uniformDynamicLampDynIMat,
        gpu.GPU_DYNAMIC_LAMP_DYNPERSMAT: uniformDynamicLampPersMat,
        gpu.GPU_DYNAMIC_LAMP_DYNENERGY: uniformDynamicLampDynEnergy,
        gpu.GPU_DYNAMIC_LAMP_DYNCOL: uniformDynamicLampDynCol,
        gpu.GPU_DYNAMIC_SAMPLER_2DBUFFER: uniformDynamicSampler2DBuffer,
        gpu.GPU_DYNAMIC_SAMPLER_2DIMAGE: uniformDynamicSampler2DImage,
        gpu.GPU_DYNAMIC_SAMPLER_2DSHADOW: uniformDynamicSampler2DShadow,
        gpu.GPU_DYNAMIC_OBJECT_AUTOBUMPSCALE: uniformDynamicObjectAutoBumpScale
    }

    #    uniform_data_type[uniform["datatype"]](uniform_types[uniform["type"]](uniform))
    return "0"


qt3dUniformsDeclaration = ["uniform mat4 projectionMatrix;",
                           "uniform mat4 inverseProjectionMatrix;",
                           "uniform mat4 modelViewMatrix;",
                           "uniform mat4 inverseModelViewMatrix;",
                           "uniform mat3 normalMatrix;"]

qt3dAttributesDeclarationGL3 = ["in vec4 vertexPosition;",
                                "in vec3 vertexNormal;"]

qt3dAttributesDeclarationGL2 = ["attribute vec4 vertexPosition;",
                                "attribute vec3 vertexNormal;"]

qt3DFragmentOuputDeclarationGL3 = ["out vec4 fragColor;"]

replacementUniforms = {"gl_ProjectionMatrix": "projectionMatrix",
                       "gl_ProjectionMatrixInverse": "inverseProjectionMatrix",
                       "gl_ModelViewMatrix": "modelViewMatrix",
                       "gl_ModelViewMatrixInverse": "inverseModelViewMatrix",
                       "gl_NormalMatrix": "normalMatrix"}

replacementAttributes = {"gl_Vertex": "vertexPosition",
                         "gl_Normal": "vertexNormal",
                         "gl_MultiTexCoord0": "vertexTexCoord"}


def preprocessVertexShaderForQt3D(vertexShader, useGL3=False):
    if useGL3:
        replacementsKeyWords = {"varying": "out",
                                "attribute": "in"}

        for i, j in replacementsKeyWords.items():
            vertexShader = vertexShader.replace(i, j)

    for i, j in replacementAttributes.items():
        vertexShader = vertexShader.replace(i, j)

    for i, j in replacementUniforms.items():
        vertexShader = vertexShader.replace(i, j)

    # Add Attributes and Uniform Declarations
    vertexShaderLines = vertexShader.split("\n")
    insertionIndex = 1

    if useGL3:
        vertexShaderLines.insert(0, "#version 330 core\n")
        for uniformDeclaration in qt3dUniformsDeclaration:
            vertexShaderLines.insert(insertionIndex, uniformDeclaration)
        for attributeDeclaration in qt3dAttributesDeclarationGL3:
            vertexShaderLines.insert(insertionIndex, attributeDeclaration)
    else:
        vertexShaderLines.insert(0, "#version 140\n")
        for uniformDeclaration in qt3dUniformsDeclaration:
            vertexShaderLines.insert(insertionIndex, uniformDeclaration)
        for attributeDeclaration in qt3dAttributesDeclarationGL2:
            vertexShaderLines.insert(insertionIndex, attributeDeclaration)

    return "\n".join(vertexShaderLines)


def preprocessFragmentShaderForQt3D(fragmentShader, useGL3=False):
    if useGL3:
        replacementsKeyWords = {"varying": "in",
                                "gl_FragColor": "fragColor"}
        for i, j in replacementsKeyWords.items():
            fragmentShader = fragmentShader.replace(i, j)

    for i, j in replacementUniforms.items():
        fragmentShader = fragmentShader.replace(i, j)

    # Add Uniform and Outputs Declarations
    fragmentShaderLines = fragmentShader.split("\n")
    insertionIndex = 0
    try:
        insertionIndex = fragmentShaderLines.index("void main(void)")
    except:
        print("FragmentShader missing main function")

    for uniformDeclaration in qt3dUniformsDeclaration:
        fragmentShaderLines.insert(insertionIndex, uniformDeclaration)

    if useGL3:
        for outputDeclaration in qt3DFragmentOuputDeclaration:
            fragmentShaderLines.insert(insertionIndex, outputDeclaration)
        fragmentShaderLines.insert(0, "#version 330 core\n")
    else:
        fragmentShaderLines.insert(0, "#version 140 core\n")

    return "\n".join(fragmentShaderLines)

class MaterialCollection(object):
    def __init__(self):
        self.materials = [Material(blenderMaterial) for blenderMaterial in bpy.data.materials]
        self.materialCollectionContent = ("import Qt3D.Core 2.9\n"
                                         "import Qt3D.Render 2.9\n"
                                         "import Qt3D.Extras 2.9\n"
                                         "\n"
                                         "Entity {\n"
                                         "    id: root\n\n"
                                         " %s \n"
                                         "}\n") % ("\n".join([str(material) for material in self.materials]))

    def __str__(self):
        return self.materialCollectionContent

    @property
    def content(self):
        return self.materialCollectionContent

    @property
    def resources(self):
        return [resource for material in self.materials for resource in material.resources]

    def materialForPropertyName(self, materialPropertyName):
        for mat in self.materials:
            if mat.materialPropertyName == materialPropertyName:
                return mat
        return None

class Material(object):
    def __init__(self, blenderMaterial, useGL3=False):
        self.resourceFiles = []
        self._blenderMaterialName = blenderMaterial.name
        self.shadersDirectoryName = "assets/shaders/"
        self._materialPropertyName = propertyName(blenderMaterial.name)

        try:
            shader = gpu.export_shader(bpy.context.scene, blenderMaterial)
            vertexShaderName = os.path.join(self.shadersDirectoryName, self._materialPropertyName.lower() + ".vert")
            self.resourceFiles.append(vertexShaderName)
            with open(vertexShaderName, "w") as f:
                f.write(preprocessVertexShaderForQt3D(shader["vertex"]))
            fragmentShaderName = os.path.join(self.shadersDirectoryName, self._materialPropertyName.lower() + ".frag")
            self.resourceFiles.append(fragmentShaderName)
            with open(fragmentShaderName, "w") as f:
                f.write(preprocessFragmentShaderForQt3D(shader["fragment"]))
            parameters = ["            Parameter { name: \"" + param["varname"] + "\"; value: " + parameterUniformValue(param) + " }" for param in shader["uniforms"]]
        except:
            pass

        self._content = ("    readonly property Material " + self._materialPropertyName + ": PhongMaterial {\n"
                         "        ambient: " + blenderColorToQColor([blenderMaterial.ambient * 0.2 for i in range(0, 3)]) + "\n"
                         "        diffuse: " + blenderColorToQColor(blenderMaterial.diffuse_color) + "\n"
                         "        specular: " + blenderColorToQColor(blenderMaterial.specular_color) + "\n"
                         "    }\n")

        # TO DO: Restore this once material generation is properly handled
        # self._content = ("    readonly property Material " + materialName + ": Material {\n"
        #               "        effect: Effect {\n"
        #               "            techniques: [\n"
        #               "                Technique {\n"
        #               "                    graphicsApiFilter {\n"
        #               "                        api: GraphicsApiFilter.OpenGL\n" +
        #              ("                        profile: GraphicsApiFilter.CoreProfile\n" +
        #               "                        majorVersion: 3\n" +
        #               "                        minorVersion: 2\n" if useGL3 else
        #               "                        profile: GraphicsApiFilter.NoProfile\n" +
        #               "                        majorVersion: 3\n" +
        #               "                        minorVersion: 0\n") +
        #                "                    }\n"
        #                "                    renderPasses: [\n"
        #                "                        RenderPass { \n"
        #                "                            shaderProgram: ShaderProgram {\n"
        #                "                                vertexShaderCode: loadSource(\"qrc:/" + vertexShaderName + "\")\n"
        #                "                                fragmentShaderCode: loadSource(\"qrc:/" + fragmentShaderName + "\")\n"
        #                "                            }\n"
        #                "                        }\n"
        #                "                    ]\n"
        #                "                }\n"
        #                "            ]\n"
        #                "        }\n"
        #                "        parameters: [\n"
        #                "%s\n"
        #                "        ]\n"
        #                "    }\n\n") % (",\n".join(parameters))

    @property
    def resources(self):
        return self.resourceFiles

    @property
    def blenderMaterialName(self):
        return self._blenderMaterialName

    @property
    def materialPropertyName(self):
        return self._materialPropertyName

    @property
    def content(self):
        return self._content

    def __str__(self):
        return self._content
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
from bpy.props import BoolProperty, FloatProperty, StringProperty, EnumProperty
from bpy_extras.io_utils import ExportHelper, orientation_helper_factory, axis_conversion
from .exporter import Exporter

# Required Blender information.
bl_info = {
           "name": "Qt3D Exporter",
           "author": "Paul Lemire <paul.lemire350@gmail.com>",
           "version": (0, 1),
           "blender": (2, 75, 0),
           "location": "File > Export > Qt3D (.qml)",
           "description": "Convert a blender project to a Qt3D Application or Qt3D files",
           "warning": "",
           "wiki_url": "",
           "tracker_url": "",
           "category": "Import-Export"
          }

OrientationHelper = orientation_helper_factory(
    "OrientationHelper", axis_forward="Z", axis_up="Y"
)

class Qt3DExporter(bpy.types.Operator, ExportHelper, OrientationHelper):
    """Qt3D Exporter"""
    bl_idname = "export_scene.qt3d_exporter";
    bl_label = "Qt3DExporter";
    bl_options = {"PRESET"};

    filename_ext = ""
    use_filter_folder = True

    # We set up exporter UI here

    use_mesh_modifiers = BoolProperty(
        name="Apply Modifiers",
        description="Apply modifiers (preview resolution)",
        default=True,
    )

    use_selection_only = BoolProperty(
        name="Selection Only",
        description="Only export selected objects",
        default=False,
    )

    use_visible_only = BoolProperty(
        name="Visible Only",
        description="Only export visible objects",
        default=False,
    )

    use_mesh_collection = BoolProperty(
        name="Use Mesh Collection",
        description="Group all Qt3D Meshes into a single file",
        default=True,
    )

    use_material_collection = BoolProperty(
        name="Use Material Collection",
        description="Group all Qt3D Materials into a single file",
        default=True,
    )

    export_full_qt3d_app = BoolProperty(
        name="Export as Qt3D Application",
        description="Creates a full Qt3D application",
        default=True,
    )

    def __init__(self):
        pass

    def draw(self, context):
        layout = self.layout
        self.properties.filepath = ""

        col = layout.box().column(align=True)
        col.label("Global", icon="QUESTION")
        col.prop(self, "export_full_qt3d_app")

        col = layout.box().column(align=True)
        col.label("Axis Conversion", icon="MANIPUL")
        col.prop(self, "axis_up")
        col.prop(self, "axis_forward")

        col = layout.box().column(align=True)
        col.label("Nodes", icon="OBJECT_DATA")
        col.prop(self, "use_selection_only")
        col.prop(self, "use_visible_only")

        col = layout.box().column(align=True)
        col.label("Meshes", icon="MESH_DATA")
        col.prop(self, "use_mesh_modifiers")
        col.prop(self, "use_mesh_collection")

        col = layout.box().column(align=True)
        col.label("Material", icon="MATERIAL_DATA")
        col.prop(self, "use_material_collection")

    def execute(self, context):
        print("In Execute" + bpy.context.scene.name)

        exportSettings = self.as_keywords()
        exportSettings["global_matrix"] = axis_conversion(to_forward=self.axis_forward, to_up=self.axis_up).to_4x4()

        self.binaryDirectoryName = "assets/binaries/"
        self.shadersDirectoryName = "assets/shaders/"
        self.qmlDirectoryName = "qml"

        # initialize progress bar
        bpy.context.window_manager.progress_begin(0, 100)
        bpy.context.window_manager.progress_update(0)

        self.userpath = self.properties.filepath
        if not os.path.isdir(self.userpath):
            self.userpath = os.path.dirname(self.userpath)
            msg = "Selecting directory: " + self.userpath
            self.report({"INFO"}, msg)


        # switch to work dir and create directories
        os.chdir(self.userpath)
        if not os.path.exists(self.binaryDirectoryName):
            os.makedirs(self.binaryDirectoryName)
        if not os.path.exists(self.shadersDirectoryName):
            os.makedirs(self.shadersDirectoryName)
        if not os.path.exists(self.qmlDirectoryName):
            os.makedirs(self.qmlDirectoryName)

        # update progress
        bpy.context.window_manager.progress_update(5)

        # Save scene into scene
        scene = bpy.context.scene

        exporter = Exporter(scene, exportSettings)
        exporter.export()

        bpy.context.window_manager.progress_update(50)

        # Create QML Files
        exporter.createQMLNodeTree()
        bpy.context.window_manager.progress_update(70)

        # Create .qrc file
        exporter.generateQRCFiles()
        bpy.context.window_manager.progress_update(80)

        if exportSettings["export_full_qt3d_app"]:
            # Create main.cpp
            exporter.generateMainCppFile()
            bpy.context.window_manager.progress_update(90)

            # Create .pro
            exporter.generateProjectFile(self.userpath)
            bpy.context.window_manager.progress_update(99)

        bpy.context.window_manager.progress_end()
        return {"FINISHED"}


def createBlenderMenu(self, context):
    self.layout.operator(Qt3DExporter.bl_idname, text="Qt3D (.qml)")


# Register against Blender
def register():
    bpy.utils.register_class(Qt3DExporter)
    bpy.types.INFO_MT_file_export.append(createBlenderMenu)


def unregister():
    bpy.utils.unregister_class(Qt3DExporter)
    bpy.types.INFO_MT_file_export.remove(createBlenderMenu)


# Handle running the script from Blender"s text editor.
if (__name__ == "__main__"):
    register()
    bpy.ops.export_scene.qt3d_exporter()

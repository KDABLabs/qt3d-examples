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

def cleanString(s):
    for i, j in { ")": "_",
                  "(": "_",
                  ".": "_",
                  " ": "_" }.items():
        s = s.replace(i, j)
    return s

def propertyName(str):
    return "".join([entry[0].upper() + entry[1:].lower() if i > 0 and len(entry) > 1 else entry.lower() for i, entry in enumerate(cleanString(str).split("_"))])

def qmlFileName(str):
    if str.lower() == "camera":
        str = "blenderCamera"
    return "".join([entry[0].upper() + entry[1:].lower() for entry in cleanString(str).split("_")])

def localMatrix4x4FromBObject(bObject):
    return "Qt.matrix4x4(" + ", ".join([(str(row.x) + ", " + str(row.y) + ", " + str(row.z) + ", " + str(row.w)) for row in bObject.matrix_local]) + ")"

def matrix4x4FromBMatrix(bMatrix):
    return "Qt.matrix4x4(" + ", ".join([(str(row.x) + ", " + str(row.y) + ", " + str(row.z) + ", " + str(row.w)) for row in bMatrix]) + ")"

def blenderColorToQColor(color):
    return "Qt.rgba(" + ", ".join([str(comp) for comp in color]) + ", 1.0)";

def blender3DVectorToQVector3D(vector):
    return "Qt.vector3d(" + ", ".join([str(comp) for comp in vector]) + ")";

"""
 Wavefront .obj 3D file to C header conversion tool
 Copyright (C) 2011-2012 Matti Dahlbom
"""
import sys
import string
import math

###############################
# Global data
###############################

# command line parameters
input_file = None
output_object_name = None
omit_textures = False
omit_colors = False
expand_object = False
center_object_around_y = None
center_object = None
object_new_height = None
object_new_width = None
cylindrical_texcoords = None
texcoords_scale_u = None
texcoords_scale_v = None

# dict of materials; a material is represented by a dict itself
# the dict keys to material_list are material names
material_list = {}
current_material = None

# list of the following tuples: (first, num, material)
# where first and num refer to indices in the face list
material_group_list = []
current_material_group = None

# The original data from v / vt / vn rows is added to these
vertex_list = []
texture_coord_list = []
vertex_normal_list = []

# current material's color
current_color = None

# used when not expanding object
vertex_indices = [] 
texture_coords = []
vertex_normals = []
vertex_colors = []

indices_datatype = "GLushort"
indices_datatype_const = "GL_UNSIGNED_SHORT"

# final list of vertices; elements are dictionaries with coord, color, 
# texcoords and normal keys. Each of the values are in turn vectors.
face_vertices = []

# number of faces currently parsed
num_faces = 0

# number of warnings in processing
num_warnings = 0

###############################
# Functions
###############################

def write_vertex_indices_list():
    # split the vertex list into lists of lists of 3 elements
    lst = [vertex_indices[i:i+3] for i in range(0, len(vertex_indices), 3)]
    s = "const %s %s_indices[] = {\n" % (indices_datatype, output_object_name)
    sep = ""
    for l in lst:
        s += "%s    %d, %d, %d" % (sep, l[0], l[1], l[2])
        sep = ",\n"
    s += "\n};\n\n"

    return s

def write_vertex_list():
    global num_warnings

    if not omit_colors:
        s = "// { x, y, z,  u, v,  r, g, b, a,  nx, ny, nz }\n"
    else:
        s = "// { x, y, z,  u, v,  nx, ny, nz }\n"
    s += "const VertexAttribs %s_vertices[] = {\n" % output_object_name

    sep = ""
    for face_vert in face_vertices:
        #print face_vert
        c = face_vert["coord"]
        tc = face_vert.get("texcoords", None)
        if tc == None:
            print "Warning: face vertex missing texture coordinates!"
            tc = (0.0, 0.0)
        n = face_vert.get("normal", None)
        if n == None:
            print "Warning: face vertex missing normal information!"
            n = (0.0, 1.0, 0.0)

        if not omit_colors:
            col = face_vert["color"]
            s += ("%s    {%s, %s, %s,\t%s, %s,\t%s, %s, %s, %s,\t%s, %s, %s}" %
                  (sep, c[0], c[1], c[2], tc[0], tc[1], 
                   col[0], col[1], col[2], col[3],
                   n[0], n[1], n[2]))
        else:
            s += ("%s    {%s, %s, %s,\t%s, %s,\t%s, %s, %s}" %
                  (sep, c[0], c[1], c[2], tc[0], tc[1], 
                   n[0], n[1], n[2]))

        sep = ",\n"

    s += "\n};\n\n"

    return s

def object_dimensions_str():
    xmin, xmax, ymin, ymax, zmin, zmax = find_dimensions()
    xsize = xmax - xmin
    ysize = ymax - ymin
    zsize = zmax - zmin
    return (("Object dimensions:\n" + 
             "  X: %f (half: %f), xmin: %f, xmax: %f\n" + 
             "  Y: %f (half: %f), ymin: %f, ymax: %f\n" + 
             "  Z: %f (half: %f), zmin: %f, zmax: %f") %
            (xsize, xsize/2, xmin, xmax,
             ysize, ysize/2, ymin, ymax,
             zsize, zsize/2, zmin, zmax))

def write_header():
    global vertex_list
    global vertex_indices
    global input_file
    global output_object_name
    global omit_textures
    
    s = """//
// Exported by obj2c.py (C) 2011-2012 Matti Dahlbom 
//

""" 
    s += "#ifndef __%s_DATA_H__\n" % output_object_name.upper()
    s += "#define __%s_DATA_H__\n\n" % output_object_name.upper()
    
    s += "// Numeric constants\n"
    s += ("static const unsigned int %sIndicesDatatype = %s;\n" %
          (output_object_name, indices_datatype_const))
    s += ("static const unsigned int %sNumVertices = %d;\n" %
          (output_object_name, len(face_vertices)))
    s += ("static const unsigned int %sNumPolygons = %d;\n" %
          (output_object_name, num_faces))
    if not expand_object:
        s += ("static const unsigned int %sNumIndices = %d;\n" %
              (output_object_name, num_faces * 3))

    s += "\n"
    
    s += "// " + object_dimensions_str().replace("\n", "\n// ") + "\n\n"

    xmin, xmax, ymin, ymax, zmin, zmax = find_dimensions()
    s += ("static const float %sHalfWidth = %f;\n" % 
          (output_object_name, ((xmax - xmin) / 2)))
    s += ("static const float %sHalfHeight = %f;\n" % 
          (output_object_name, ((ymax - ymin) / 2)))
    s += ("static const float %sHalfDepth = %f;\n" % 
          (output_object_name, ((zmax - zmin) / 2)))
    s += "\n"

    if not expand_object:
        # write the vertex indices list for glDrawElements() to use
        s += write_vertex_indices_list()

    # write the actual vertex/texcoord/color lists 
    s += write_vertex_list()

    s += "#endif\n\n"

    # write the .h file to disk
    file_out_name = "%s_data.h" % output_object_name
    file_out = open(file_out_name, "w")
    file_out.write(s)
    file_out.close()
    print "Wrote %s" % file_out_name

def strip_paths(w):
    index = w.rfind("/")
    if index != -1:
        w = w[index + 1:]
    index = w.rfind("\\")
    if index != -1:
        w = w[index + 1:]

    return w

def process_mtllib_line(words):
    global material_list
    global current_material

    if words[0] == "#":
        # skip comment line
        return
    elif words[0] == "newmtl":
        if current_material:
            material_list[current_material["name"]] = current_material
        current_material = {"name": " ".join(words[1:])}
    elif words[0] == "Ka":
        current_material["ambient_color"] = (words[1], words[2], words[3])
    elif words[0] == "Kd":
        current_material["diffuse_color"] = (words[1], words[2], words[3])
    elif words[0] == "Ks":
        current_material["specular_color"] = (words[1], words[2], words[3])
    elif words[0] == "Ni":
        current_material["specular_coeff"] = words[1]
    elif words[0] == "d":
        current_material["transparency"] = words[1]
    elif words[0] == "illum":
        current_material["illumination_model"] = words[1]
    elif words[0] == "map_Ka" or words[0] == "map_Kd":
        if not omit_textures:
            current_material["texture_map"] = strip_paths(words[1])
    #TODO other map types: map_Ks, map_d, bump, map_bump

def process_mtllib(name):
    global material_list
    global current_material

    mtllib_file_in = open(name, "r")
    line = mtllib_file_in.readline()
    while line:
        if line.strip() != "":
            process_mtllib_line(line.split())
        line = mtllib_file_in.readline()
    
    mtllib_file_in.close()

    # append last material, if any
    if current_material:
        material_list[current_material["name"]] = current_material
        current_material = None

def to_byte_color(c):
    return [int(255 * float(c[0])), int(255 * float(c[1])), 
            int(255 * float(c[2])), 255]

def find_material(i):
    """ 
    Looks up a material by index to the vertex_indices table (i).
    """
    for f, n, mat in material_group_list:
        first = f * 3 
        last = (f + n) * 3 - 1
        #print "find_material(): %s %d..%d" % (mat["name"], first, last)
        if i >= first and i <= last:
            return mat

    print "find_material(): could not find material for i = %d!" % i
    return None

def materials_match(m1, m2):
    if m1["name"] == m2["name"]:
        return True
    
    if m1.has_key("texture_map") and m2.has_key("texture_map"):
        if m1["texture_map"] != m2["texture_map"]:
            return False

    if m1["ambient_color"] != m2["ambient_color"]:
        return False

    if m1["diffuse_color"] != m2["diffuse_color"]:
        return False
    
    return True

def separate_material_group_vertices():
    """ 
    Find any vertices shared between material groups and clone new vertices
    to break any shared vertices between the groups.
    """
    global vertex_indices
    global vertex_list

    print "Finding vertices shared between material groups.."
    num_new_vertices = 0

    for f, n, mat in material_group_list:
        print "-> checking mat '%s' against others.." % mat["name"]
        first = f * 3 
        last = (f + n) * 3 - 1

        # go through vertex indices of this material group
        for i in range(first, last + 1):
            # ..and compare them to the vertex indices in all the other groups
            for j in range(len(vertex_indices)):
                if j < first or j > last:
                    if vertex_indices[i] == vertex_indices[j]:
                        other_mat = find_material(j)
                        if other_mat:
                            if materials_match(mat, other_mat):
                                continue
                        
                        # shared vertex found; allocate new vertex and rewrite
                        # vertex indices table
                        new_index = len(vertex_list)
                        vertex_list.append(vertex_list[vertex_indices[i]])
                        vertex_indices[i] = new_index
                        num_new_vertices = num_new_vertices + 1
                        break

    if num_new_vertices > 0:
        print "%d new vertices added." % num_new_vertices

def process_face(parts):
    global current_color
    global omit_textures
    global num_faces
    global face_vertices
    global vertex_indices
    global texture_coords
    global vertex_normals
    global vertex_colors

    """
    Parse f records; they can be any kind of the following:
      f 1 2 3
      f 3/1 4/2 5/3
      f 6/4/1 3/5/3 7/6/5
      f 6//1 3//3 7//5
      
      with order being v/vt/vn; only v has to be present.
      v = vertex index
      vt = texture coordinate index
      vn = vertex normal index
      
      All indices start from 1 so we need to substract 1 from each.
    """
    if len(parts) != 3:
        print "process_face(): only triangles supported!"
        return

    indices = []
    coords = []
    texcoords = []
    normals = []

    for part in parts:
        p = part.split("/")
        indices.append(int(p[0]) - 1)
        coords.append(vertex_list[int(p[0]) - 1]) 
        if len(p) > 1:
            if p[1] != "":
                texcoords.append(texture_coord_list[int(p[1]) - 1]);
            else:
                # texture coordinate index not present
                texcoords.append((0.0, 0.0))

        if len(p) > 2:
            normals.append(vertex_normal_list[int(p[2]) - 1])

    byte_color = [0, 0, 0, 0]
    if not omit_colors:
        byte_color = to_byte_color(current_color)
        if current_material_group[2].has_key("texture_map"):
            # this face is textured; set color alpha to 0
            byte_color[3] = 0

    if expand_object:
        for i in range(3):
            face_vertex = {}
            face_vertex["color"] = byte_color;
            face_vertex["coord"] = coords[i]
            face_vertex["texcoords"] = texcoords[i] 
            face_vertex["normal"] = normals[i]
            face_vertices.append(face_vertex)
    else:
        vertex_indices.extend(indices)
        texture_coords.extend(texcoords)
        vertex_normals.extend(normals)
        if not omit_colors:
            vertex_colors.extend((byte_color, byte_color, byte_color))

    num_faces = num_faces + 1

def create_color(ambient_color, diffuse_color):
    """ 
    Calculates a single color value from ambient & diffuse color 
    components by mostly adding them together and clamping the values.
    Both color parameters (and the return value) are vectors of 3 floats.
    """
    if len(ambient_color) != 3 or len(diffuse_color) != 3:
        print "create_color(): invalid parameters."
        return (0.0, 0.0, 0.0)

    ret_col = []
    for i in range(3):
        c = float(ambient_color[i]) + float(diffuse_color[i])
        if c > 1.0:
            c = 1.0
        ret_col.append(c)

    return ret_col

def add_current_material_group():
    global material_group_list
    global current_material_group

    if current_material_group:
        num = num_faces - current_material_group[0]
        current_material_group = (current_material_group[0], num, 
                                 current_material_group[2])
        material_group_list.append(current_material_group)
        print ("Added material group '%s': first face = %d, num faces = %d" %
               (current_material_group[2]["name"], current_material_group[0], 
                current_material_group[1]))

def handle_material_group(mtl_name):
    global material_list
    global current_material_group
    global material_group
    global current_color

    material = material_list[mtl_name]

    # update current color
    current_color = create_color(material["ambient_color"], 
                                 material["diffuse_color"])

    # add previous material group to the group list
    add_current_material_group()

    # start a new material group
    print "Using material '%s'.." % mtl_name
    current_material_group = (num_faces, 0, material)

def process_line(words):
    global vertex_list
    global texture_coord_list
    global vertex_normal_list

    if words[0] == "#":
        # skip comment line
        return
    elif words[0] == "o":
        print "Parsing object '%s'.." % " ".join(words[1:])
    elif words[0] == "mtllib":
        # process material library file
        process_mtllib(words[1])
    elif words[0] == "v":
        vertex_list.append((float(words[1]), float(words[2]), float(words[3])))
    elif words[0] == "vt":
        texture_coord_list.append((float(words[1]), float(words[2])))
    elif words[0] == "vn":
        vertex_normal_list.append((float(words[1]), float(words[2]), 
                               float(words[3])))
    elif words[0] == "usemtl":
        handle_material_group(" ".join(words[1:]))
    elif words[0] == "f":
        process_face(words[1:])

def find_dimensions():
    # Sanity checking
    if len(face_vertices) == 0:
        print "Error: face_vertices list is empty!"
        return

    xmin = float("inf")
    xmax = float("-inf")
    ymin = float("inf")
    ymax = float("-inf")
    zmin = float("inf")
    zmax = float("-inf")

    for f in face_vertices:
        x, y, z = f["coord"]

        if y < ymin:
            ymin = y
        if y > ymax:
            ymax = y
        if x < xmin:
            xmin = x
        if x > xmax:
            xmax = x
        if z < zmin:
            zmin = z
        if z > zmax:
            zmax = z

    return (xmin, xmax, ymin, ymax, zmin, zmax)

def scale_object(scaler):
    i = 0
    while i < len(face_vertices):
        face_vert = face_vertices[i]
        c = face_vert["coord"]
        face_vert["coord"] = [x * scaler for x in c]
        i += 1

def scale_to_height(height):
    print "Scaling the object to height of %f.." % height

    ymin, ymax = find_dimensions()[2:4]
    scaler = height / (ymax - ymin)
    scale_object(scaler)

def scale_to_width(width):
    print "Scaling the object to width of %f.." % width

    xmin, xmax = find_dimensions()[0:2]
    scaler = width / (xmax - xmin)
    scale_object(scaler)

def center_around_y():
    print "Setting minimum y = 0 and centering object around y axis.."

    xmin, xmax, ymin, ymax, zmin, zmax = find_dimensions()

    xcenter = (xmax + xmin) / 2
    zcenter = (zmax + zmin) / 2
    print "Translating on XZ plane by x: %f, z: %f" % (-xcenter, -zcenter)

    i = 0
    while i < len(face_vertices):
        face_vert = face_vertices[i]
        x, y, z = face_vert["coord"]
        face_vert["coord"] = (x - xcenter, y - ymin, z - zcenter)
        i += 1

def scale_texcoords(u_scale, v_scale):
    if u_scale == None:
        u_scale = 1.0
    if v_scale == None:
        v_scale = 1.0

    print "Scaling texture (U,V)s by (%f,%f).." % (u_scale, v_scale)

    i = 0
    while i < len(face_vertices):
        face_vert = face_vertices[i]
        u, v = face_vert["texcoords"]
        face_vert["texcoords"] = (u * u_scale, v * v_scale)
        i += 1

def center():
    print "Centering object around (0.0, 0.0, 0.0).."

    xmin, xmax, ymin, ymax, zmin, zmax = find_dimensions()
    xcenter = (xmax + xmin) / 2
    ycenter = (ymax + ymin) / 2
    zcenter = (zmax + zmin) / 2

    print ("Translating geometry by (%f, %f, %f)" % 
           (-xcenter, -ycenter, -zcenter))

    i = 0
    while i < len(face_vertices):
        face_vert = face_vertices[i]
        x, y, z = face_vert["coord"]
        face_vert["coord"] = (x - xcenter, y - ycenter, z - zcenter)
        i += 1

def create_indexed_vertex_list():
    global face_vertices

    print "Creating indexed vertex data list.."
    
    # initialize the face_vertex by putting the vertex coords in
    for i in range(len(vertex_list)):
        vertex = {}
        vertex["coord"] = vertex_list[i]
        face_vertices.append(vertex)
    
    for i in range(len(vertex_indices)):
        index = vertex_indices[i]
        vertex = face_vertices[index]
        # put in color, texcoords, normal
        if not omit_colors:
            vertex["color"] = vertex_colors[i]
        vertex["texcoords"] = texture_coords[i]
        vertex["normal"] = vertex_normals[i]

def generate_cylindrical_texcoords():
    ymin, ymax = find_dimensions()[2:4]

    print "Generating cylindrical u,v texture coordinates.."
    i = 0
    while i < len(face_vertices):
        face_vert = face_vertices[i]
        x, y, z = face_vert["coord"]

        # Calculate cylindrical projection u,v like so:
        # u = (y - ymin) / (ymax - ymin) (linear 0..1 from coord height)
        # v = (atan2(z,x) + PI) / 2PI (0..1 from angle around y axis)
        u = (y - ymin) / (ymax - ymin)
        v = (math.atan2(z, x) + math.pi) / (2 * math.pi)

        face_vert["texcoords"] = (u, v)
        i += 1

def print_object_info():
    if len(material_group_list) > 0:
        print "Polygon faces (first, num) and their materials:"
        for f, n, m in material_group_list:
            if m.has_key("texture_map"):
                print ("  %d, %d, material='%s', texture='%s'" % 
                       (f, n, m["name"], m["texture_map"]))
            else:
                print ("  %d, %d, material='%s', diffuse color=%s" % 
                       (f, n, m["name"], 
                        str(to_byte_color(m["diffuse_color"])[0:3])))

    print "Total %d vertices" % len(face_vertices)
    print "Total %d polygons (GL_TRIANGLE)" % num_faces
    print "%d warnings in processing" % num_warnings
    print object_dimensions_str()

def parse_cmdline(argv):
    global input_file
    global output_object_name
    global omit_textures
    global omit_colors
    global expand_object
    global center_object_around_y
    global center_object
    global object_new_height
    global object_new_width
    global cylindrical_texcoords
    global texcoords_scale_u
    global texcoords_scale_v

    index = 1
    for i in range(len(argv))[1:]:
        arg = argv[index]

        if arg[0] != '-':
            if not input_file:
                input_file = arg
            else:
                output_object_name = arg
        else:
            if arg == "-ot":
                omit_textures = True
            elif arg == "-oc":
                omit_colors = True
            elif arg == "-e":
                expand_object = True
            elif arg == "-cy":
                center_object_around_y = True
            elif arg == "-c":
                center_object = True
            elif arg == "-sy":
                object_new_height = float(argv[index + 1])
                index += 1
            elif arg == "-sx":
                object_new_width = float(argv[index + 1])
                index += 1
            elif arg == "-su":
                texcoords_scale_u = float(argv[index + 1])
                index += 1
            elif arg == "-sv":
                texcoords_scale_v = float(argv[index + 1])
                index += 1
            elif arg == "-cyl":
                cylindrical_texcoords = True

        # Next arg
        index += 1
        if index >= len(argv):
            break

    if not input_file or not output_object_name:
        print ("Usage: python %s [options] input_file_name output_object_name" %
               argv[0])
        print "options include:"
        print "\t\t-ot\tomit textures"
        print "\t\t-oc\tomit colors"
        print "\t\t-cy\tcenter object around Y axis and set min y = 0.0"
        print "\t\t-c\tcenter object"
        print "\t\t-sy <n>\tset object's Y height to <n> (will scale coords)"
        print "\t\t-sx <n>\tset object's X width to <n> (will scale coords)"
        print "\t\t-su <n>\tscale all texture Us by n"
        print "\t\t-sv <n>\tscale all texture Vs by n"
        print "\t\t-cyl\tcalculate cylindrical texture coords around Y axis"
        print "\t\t-e\t\"expand\" object (no shared vertices; for " \
            "glDrawArrays())"

        return False

    return True

def main():
    if not parse_cmdline(sys.argv):
        return 

    # read a file defined on command line line by line and split each line 
    # to words
    print "input file = %s" % input_file
    print "output object name = %s" % output_object_name
    
    if center_object_around_y and center_object:
        print "Error: cannot declare both -c and -cy"
        return

    if object_new_height and object_new_width:
        print "Error: can declare only one of: -sy, -sx"
        return

    if omit_textures:
        print "Omitting textures."
    if expand_object:
        print "Expanding object: no shared vertices."

    file_in = open(input_file, "r")
    line = file_in.readline()
    while line:
        if line.strip() != "":
            process_line(line.split())
            line = file_in.readline()

    file_in.close()

    # add the last material group to the group list
    add_current_material_group()

    # for non-expanded objects we must now create the final face vertex list
    # using the vertex indices & texcoord/color/normal arrays
    if not expand_object:
        # first we'll break vertex sharing relationships between material 
        # groups to allow each group be drawn with their own color
        separate_material_group_vertices()

        # Create a face_vertices list to be indexed
        create_indexed_vertex_list()

    # After this point, face_vertices is always populated and all operations
    # must focus on it instead of other lists

    # Generate cylindrical projection texture coordinates if specified
    if cylindrical_texcoords:
        generate_cylindrical_texcoords()

    # scale object to given height if -sy specified
    if object_new_height != None:
        scale_to_height(object_new_height)

    # scale object to given width if -sx specified
    if object_new_width != None:
        scale_to_width(object_new_width)

    # center object around Y axis if specified
    if center_object_around_y:
        center_around_y()
        
    # center object if specified
    if center_object:
        center()

    # scale texture U,V if specified
    if texcoords_scale_u != None or texcoords_scale_v != None:
        scale_texcoords(texcoords_scale_u, texcoords_scale_v)
        
    # Print object information
    print_object_info()

    # finally write the .h file to disk
    write_header();

if __name__ == "__main__":
    main()



#include "models.hpp"

// Read OFF models with triangle faces. Comments are allowed.
// -
// Ported from my own C code.
Model *load_OFF_model(std::string const &filename, float scale, Point center, bool invert_winding_order, bool create_phong_normals)
{   
    std::cout << "Loading model \"" << filename << "\" with parameters:\n";
    std::cout << "    scale: " << scale << "\n";
    std::cout << "    center: " << center << "\n";
    #define load_error(ERROR_STRING) {\
        fprintf(stderr, "Error loading OFF file: " ERROR_STRING "\n");\
        exit(EXIT_FAILURE);\
    }
    FILE *file = fopen(filename.c_str(), "r");
    if (file == NULL) load_error("File doesn't exist.");
    // std::fstream file;
    // file.open(filename);
    // if (!file.is_open()) {
    //     load_error("File doesn't exist.");
    // }
    #define line_size 512
    char line_buffer[line_size];
    #define get_line() {\
        while (1) {\
            if (fgets(line_buffer, line_size, file) == NULL) load_error("Couldn't get a line.");\
            break;\
        }\
    }
    get_line();
    if (strcmp(line_buffer, "OFF\n") != 0) load_error("OFF files must start with the line \"OFF\".");
    get_line();
    int num_vertices, num_faces, num_edges;
    if (sscanf(line_buffer, "%d %d %d\n", &num_vertices, &num_faces, &num_edges) == EOF) load_error("Line should consist of 3 numbers, for #vertices, #faces, #edges.");

    std::vector<Point> vertices(num_vertices);

    for (int i = 0; i < num_vertices; i++) {
        get_line();
        float x,y,z;
        if (sscanf(line_buffer, "%f %f %f\n", &x, &y, &z) == EOF) load_error("Malformed vertex entry.");
        vertices[i] = Point(scale*(x - center.x),
                            scale*(y - center.y),
                            scale*(z - center.z));
    }
    std::vector<uint16_t> triangles(3 * num_faces);
    for (int i = 0; i < num_faces; i++) {
        get_line();
        uint16_t a,b,c;
        if (sscanf(line_buffer, "3 %hu %hu %hu\n", &a, &b, &c) == EOF) load_error("Malformed face entry. Faces must be triangles.");
        if (invert_winding_order) {
            triangles[3*i + 0] = c;
            triangles[3*i + 1] = b;
            triangles[3*i + 2] = a;
        } else {
            triangles[3*i + 0] = a;
            triangles[3*i + 1] = b;
            triangles[3*i + 2] = c;
        }
    }
    // vector<>s should be on the heap, so won't be destroyed. (?)
    Model *model = new Model(vertices, num_vertices, triangles, num_faces);
    if (create_phong_normals) {
        model->has_normals = true;
        model->normals = vector<Vector>(model->num_vertices);
        for (int i = 0; i < model->num_vertices; i++) {
            model->normals[i] = Vector(0,0,0);
        }
        for (int i = 0; i < model->num_triangles; i++) {
            Point &a = model->vertices[model->triangles[3*i+0]];
            Point &b = model->vertices[model->triangles[3*i+1]];
            Point &c = model->vertices[model->triangles[3*i+2]];
            Vector n = glm::cross(c-a, b-a);
            model->normals[model->triangles[3*i+0]] += n;
            model->normals[model->triangles[3*i+1]] += n;
            model->normals[model->triangles[3*i+2]] += n;
        }
        for (int i = 0; i < model->num_vertices; i++) {
            model->normals[i] = glm::dot(model->normals[i], model->normals[i]) < 1e-6 ? Vector(0,0,0) // shouldn't happen
                                : glm::normalize(model->normals[i]);
        }
    }

    model->print_properties();
    return model;
}


void Model::print_properties() const
{
    std::cout << "Model properties:\n";
    std::cout << "    num_vertices: " << num_vertices << "\n";
    std::cout << "    num_triangles: " << num_triangles << "\n";
}


// copy() and transform_by() are primarily for baking a model into the scene, in world space,
// so rays don't have to be transformed into its object space.
Model *Model::copy()
{
    Model *new_model = new Model();
    new_model->num_vertices = num_vertices;
    new_model->has_normals = has_normals;
    new_model->num_triangles = num_triangles;
    // Remember to copy every dynamic object!
    // There probably is a better way to do this.
    new_model->vertices = vector<Point>(num_vertices);
    for (int i = 0; i < num_vertices; i++) new_model->vertices[i] = vertices[i];
    new_model->triangles = vector<uint16_t>(3*num_triangles);
    for (int i = 0; i < 3*num_triangles; i++) new_model->triangles[i] = triangles[i];

    if (has_normals) {
        new_model->normals = vector<Vector>(num_vertices);
        for (int i = 0; i < num_vertices; i++) new_model->normals[i] = normals[i];
    }
    return new_model;
}
void Model::transform_by(const Transform &transform)
{
    //---If normals are held, transform these.
    for (int i = 0; i < num_vertices; i++) {
        vertices[i] = transform(vertices[i]);
    }
}

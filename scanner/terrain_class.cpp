#include "headers/terrain_class.h"
TerrainClass::TerrainClass()
{
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
    terrain_filename_ = nullptr;
    color_map_filename_ = nullptr;
    height_map_ = nullptr;
    terrain_model_ = nullptr;
    terrain_cells_ = nullptr;
}

TerrainClass::TerrainClass(const TerrainClass&)
{
    
}

TerrainClass::~TerrainClass()
{
    
}

bool TerrainClass::Initialize(ID3D11Device* device, char* setup_filename)
{
    bool result;

    result = Load_setup_file(setup_filename);
    if (!result)
        return false;

    result = Load_raw_height_map();
    if (!result)
        return false;

    Set_terrain_coordinates();

    result = Calculate_normals();
    if (!result)
        return false;

    result = Load_color_map();
    if (!result)
        return false;

    result = Build_terrain_model();
    if (!result)
        return false;

    Shutdown_height_map();

    Calculate_terrain_vectors();

    result = Load_terrain_cells(device);
    if (!result)
        return false;

    Shutdown_terrain_model();

    return true;
}

void TerrainClass::Shutdown()
{
    Shutdown_terrain_cells();
    Shutdown_terrain_model();
    Shutdown_height_map();
}

void TerrainClass::Frame()
{
    render_count_ = 0;
    cells_drawn_ = 0;
    cells_culled_ = 0;
}

bool TerrainClass::Load_setup_file(char* filename)
{
    int string_len;
    ifstream fin;
    char input;

    string_len = 256;
    terrain_filename_ = new char[string_len];
    if (!terrain_filename_)
        return false;

    color_map_filename_ = new char[string_len];
    if (!color_map_filename_)
        return false;

    fin.open(filename);
    if (fin.fail())
        return false;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin >> terrain_filename_;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin >> terrain_height_;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin >> terrain_width_;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin >> height_scale_;

    fin.get(input);
    while (input != ':')
        fin.get(input);
    fin >> color_map_filename_;

    fin.close();
    return true;
}

bool TerrainClass::Load_bitmap_height_map()
{
    int error, image_size, i, j, k, index;
    FILE* file_ptr;
    unsigned long long count;
    BITMAPFILEHEADER bitmap_file_header;
    BITMAPINFOHEADER bitmap_info_header;
    unsigned char* bitmap_image;
    unsigned char height;

    height_map_ = new HEIGHT_MAP_TYPE[terrain_width_ * terrain_height_];
    if (!height_map_)
        return false;

    error = fopen_s(&file_ptr, terrain_filename_, "rb");
    if (error != 0)
        return false;

    count = fread(&bitmap_file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    count = fread(&bitmap_info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    if ((bitmap_info_header.biHeight != terrain_height_) || (bitmap_info_header.biWidth != terrain_width_))
        return false;

    //need to add extra coz 257x257 non-divide-able
    image_size = terrain_height_ * ((terrain_width_ * 3) + 1);

    bitmap_image = new unsigned char[image_size];
    if (!bitmap_image)
        return false;

    //beginning of bitmap data
    fseek(file_ptr, bitmap_file_header.bfOffBits, SEEK_SET);
    count = fread(bitmap_image, 1, image_size, file_ptr);
    if (count != image_size)
        return false;

    error = fclose(file_ptr);
    if (error != 0)
        return false;

    k = 0;
    for (j = 0; j < terrain_height_; j++)
    {
        for (i = 0; i < terrain_width_; i++)
        {
            index = (terrain_width_ * (terrain_height_ - 1 - j)) + i;
            height = bitmap_image[k];
            height_map_[index].y = (float)height;
            k += 3;
        }
        k++;
    }
    delete[] bitmap_image;
    bitmap_image = nullptr;

    delete[] terrain_filename_;
    terrain_filename_ = nullptr;

    return true;
}

bool TerrainClass::Load_raw_height_map()
{
    int error, i, j, index;
    FILE* file_ptr;
    unsigned long long image_size, count;
    unsigned short* raw_image;

    height_map_ = new HEIGHT_MAP_TYPE[terrain_width_ * terrain_height_];
    if (!height_map_)
        return false;

    error = fopen_s(&file_ptr, terrain_filename_, "rb");
    if (error != 0)
        return false;

    image_size = terrain_height_ * terrain_width_;

    raw_image = new unsigned short[image_size];
    if (!raw_image)
        return false;

    count = fread(raw_image, sizeof(unsigned short), image_size, file_ptr);
    if (count != image_size)
        return false;

    error = fclose(file_ptr);
    if (error != 0)
        return false;

    for (j = 0; j < terrain_height_; j++)
    {
        for (i = 0; i < terrain_width_; i++)
        {
            index = (terrain_width_ * j) + i;
            height_map_[index].y = (float)raw_image[index];
        }
    }
    delete[] raw_image;
    raw_image = nullptr;

    delete[] terrain_filename_;
    terrain_filename_ = nullptr;

    return true;
}

void TerrainClass::Shutdown_height_map()
{
    if (height_map_)
    {
        delete[] height_map_;
        height_map_ = nullptr;
    }
}

//bitmap is upside down
void TerrainClass::Set_terrain_coordinates()
{
    int i, j, index;

    for (j = 0; j < terrain_height_; j++)
    {
        for (i = 0; i < terrain_width_; i++)
        {
            index = (terrain_width_ * j) + i;

            height_map_[index].x = (float)i - 512;
            height_map_[index].z = -(float)j;
            height_map_[index].z += (float)(terrain_height_ - 312);
            height_map_[index].y /= height_scale_;
            height_map_[index].y -= 12;

        }
    }
}

bool TerrainClass::Calculate_normals()
{
    int i, j, index1, index2, index3, index;
    float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
    VECTOR_TYPE* normals;

    normals = new VECTOR_TYPE[(terrain_height_ - 1) * (terrain_width_ - 1)];
    if (!normals)
        return false;

    for (j = 0; j < (terrain_height_ - 1); j++)
    {
        for (i = 0; i < (terrain_width_ - 1); i++)
        {
            index1 = ((j + 1) * terrain_width_) + i;
            index2 = ((j + 1) * terrain_width_) + (i + 1);
            index3 = ((j)* terrain_width_) + i;

            vertex1[0] = height_map_[index1].x;
            vertex1[1] = height_map_[index1].y;
            vertex1[2] = height_map_[index1].z;

            vertex2[0] = height_map_[index2].x;
            vertex2[1] = height_map_[index2].y;
            vertex2[2] = height_map_[index2].z;

            vertex3[0] = height_map_[index3].x;
            vertex3[1] = height_map_[index3].y;
            vertex3[2] = height_map_[index3].z;

            vector1[0] = vertex1[0] - vertex3[0];
            vector1[1] = vertex1[1] - vertex3[1];
            vector1[2] = vertex1[2] - vertex3[2];
            vector2[0] = vertex3[0] - vertex2[0];
            vector2[1] = vertex3[1] - vertex2[1];
            vector2[2] = vertex3[2] - vertex2[2];

            index = (j * (terrain_width_ - 1)) + i;

            normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
            normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
            normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

            length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) + (normals[index].z * normals[index].z));

            normals[index].x = (normals[index].x / length);
            normals[index].y = (normals[index].y / length);
            normals[index].z = (normals[index].z / length);
        }
    }

    for (j = 0; j < terrain_height_; j++)
    {
        for (i = 0; i < terrain_width_; i++)
        {
            sum[0] = 0.0f;
            sum[1] = 0.0f;
            sum[2] = 0.0f;

            if (((i - 1) >= 0) && ((j - 1) >= 0))
            {
                index = ((j - 1) * (terrain_width_ - 1)) + (i - 1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if ((i < (terrain_width_ - 1)) && ((j - 1) >= 0))
            {
                index = ((j - 1) * (terrain_width_ - 1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if (((i - 1) >= 0) && (j < terrain_height_ - 1))
            {
                index = (j * (terrain_width_ - 1)) + (i - 1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if ((i < (terrain_width_ - 1)) && (j < terrain_height_ - 1))
            {
                index = (j * (terrain_width_ - 1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
            index = (j * terrain_width_) + i;

            height_map_[index].nx = (sum[0] / length);
            height_map_[index].ny = (sum[1] / length);
            height_map_[index].nz = (sum[2] / length);
        }
    }
    delete[] normals;
    normals = nullptr;

    return true;
}

bool TerrainClass::Load_color_map()
{
    int error, image_size, i, j, k, index;
    FILE* file_ptr;
    unsigned long long count;
    BITMAPFILEHEADER bitmap_file_header;
    BITMAPINFOHEADER bitmap_info_header;
    unsigned char* bitmap_image;
    
    error = fopen_s(&file_ptr, color_map_filename_, "rb");
    if (error != 0)
        return false;

    count = fread(&bitmap_file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    count = fread(&bitmap_info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    if ((bitmap_info_header.biHeight != terrain_height_) || (bitmap_info_header.biWidth != terrain_width_))
        return false;

    //need to add extra coz 257x257 non-divide-able
    image_size = terrain_height_ * ((terrain_width_ * 3) + 1);

    bitmap_image = new unsigned char[image_size];
    if (!bitmap_image)
        return false;

    //beginning of bitmap data
    fseek(file_ptr, bitmap_file_header.bfOffBits, SEEK_SET);
    count = fread(bitmap_image, 1, image_size, file_ptr);
    if (count != image_size)
        return false;

    error = fclose(file_ptr);
    if (error != 0)
        return false;

    k = 0;
    for (j = 0; j < terrain_height_; j++)
    {
        for (i = 0; i < terrain_width_; i++)
        {
            index = (terrain_width_ * (terrain_height_ - 1 - j)) + i;
            height_map_[index].b = (float)bitmap_image[k] / 255.0f;
            height_map_[index].g = (float)bitmap_image[k + 1] / 255.0f;
            height_map_[index].r = (float)bitmap_image[k + 2] / 255.0f;
            k += 3;
        }
        //its for compenaste extra byte at end of each line in non modulo 2 bitmaps
        k++;
    }
    delete[] bitmap_image;
    bitmap_image = nullptr;

    delete[] color_map_filename_;
    color_map_filename_ = nullptr;

    return true;
}

bool TerrainClass::Build_terrain_model()
{
    int i, j, index, index1, index2, index3, index4;
    float quads_covered, increment_size, tu2left, tu2right, tv2bottom, tv2top;

    vertex_count_ = (terrain_height_ - 1) * (terrain_width_ - 1) * 6;

    terrain_model_ = new MODEL_TYPE[vertex_count_];
    if (!terrain_model_)
        return false;

    //fixed 33x33 vertex array
    quads_covered = 32.0f;
    increment_size = 1.0f / quads_covered;

    tu2left = 0.0f;
    tu2right = increment_size;
    tv2top = 0.0f;
    tv2bottom = increment_size;

    //triangle strip
    index = 0;
    for (j = 0; j < (terrain_height_ - 1); j++)
    {
        for (i = 0; i < (terrain_width_ - 1); i++)
        {
            index1 = (terrain_width_ * j) + i;
            index2 = (terrain_width_ * j) + (i + 1);
            index3 = (terrain_width_ * (j + 1)) + i;
            index4 = (terrain_width_ * (j + 1)) + (i + 1);

            terrain_model_[index].x = height_map_[index1].x;
            terrain_model_[index].y = height_map_[index1].y;
            terrain_model_[index].z = height_map_[index1].z;
            terrain_model_[index].tu = 0.0f;
            terrain_model_[index].tv = 0.0f;
            terrain_model_[index].nx = height_map_[index1].nx;
            terrain_model_[index].ny = height_map_[index1].ny;
            terrain_model_[index].nz = height_map_[index1].nz;
            terrain_model_[index].r = height_map_[index1].r;
            terrain_model_[index].g = height_map_[index1].g;
            terrain_model_[index].b = height_map_[index1].b;
            terrain_model_[index].tu2 = tu2left;
            terrain_model_[index].tv2 = tv2top;
            index++;

            terrain_model_[index].x = height_map_[index2].x;
            terrain_model_[index].y = height_map_[index2].y;
            terrain_model_[index].z = height_map_[index2].z;
            terrain_model_[index].tu = 1.0f;
            terrain_model_[index].tv = 0.0f; 
            terrain_model_[index].nx = height_map_[index2].nx;
            terrain_model_[index].ny = height_map_[index2].ny;
            terrain_model_[index].nz = height_map_[index2].nz;
            terrain_model_[index].r = height_map_[index2].r;
            terrain_model_[index].g = height_map_[index2].g;
            terrain_model_[index].b = height_map_[index2].b;
            terrain_model_[index].tu2 = tu2right;
            terrain_model_[index].tv2 = tv2top;
            index++;

            terrain_model_[index].x = height_map_[index3].x;
            terrain_model_[index].y = height_map_[index3].y;
            terrain_model_[index].z = height_map_[index3].z;
            terrain_model_[index].tu = 0.0f;
            terrain_model_[index].tv = 1.0f;
            terrain_model_[index].nx = height_map_[index3].nx;
            terrain_model_[index].ny = height_map_[index3].ny;
            terrain_model_[index].nz = height_map_[index3].nz;
            terrain_model_[index].r = height_map_[index3].r;
            terrain_model_[index].g = height_map_[index3].g;
            terrain_model_[index].b = height_map_[index3].b;
            terrain_model_[index].tu2 = tu2left;
            terrain_model_[index].tv2 = tv2bottom;
            index++;

            terrain_model_[index].x = height_map_[index3].x;
            terrain_model_[index].y = height_map_[index3].y;
            terrain_model_[index].z = height_map_[index3].z;
            terrain_model_[index].tu = 0.0f;
            terrain_model_[index].tv = 1.0f;
            terrain_model_[index].nx = height_map_[index3].nx;
            terrain_model_[index].ny = height_map_[index3].ny;
            terrain_model_[index].nz = height_map_[index3].nz;
            terrain_model_[index].r = height_map_[index3].r;
            terrain_model_[index].g = height_map_[index3].g;
            terrain_model_[index].b = height_map_[index3].b;
            terrain_model_[index].tu2 = tu2left;
            terrain_model_[index].tv2 = tv2bottom;
            index++;

            terrain_model_[index].x = height_map_[index2].x;
            terrain_model_[index].y = height_map_[index2].y;
            terrain_model_[index].z = height_map_[index2].z;
            terrain_model_[index].tu = 1.0f;
            terrain_model_[index].tv = 0.0f;
            terrain_model_[index].nx = height_map_[index2].nx;
            terrain_model_[index].ny = height_map_[index2].ny;
            terrain_model_[index].nz = height_map_[index2].nz;
            terrain_model_[index].r = height_map_[index2].r;
            terrain_model_[index].g = height_map_[index2].g;
            terrain_model_[index].b = height_map_[index2].b;
            terrain_model_[index].tu2 = tu2right;
            terrain_model_[index].tv2 = tv2top;
            index++;

            terrain_model_[index].x = height_map_[index4].x;
            terrain_model_[index].y = height_map_[index4].y;
            terrain_model_[index].z = height_map_[index4].z;
            terrain_model_[index].tu = 1.0f;
            terrain_model_[index].tv = 1.0f;
            terrain_model_[index].nx = height_map_[index4].nx;
            terrain_model_[index].ny = height_map_[index4].ny;
            terrain_model_[index].nz = height_map_[index4].nz;
            terrain_model_[index].r = height_map_[index4].r;
            terrain_model_[index].g = height_map_[index4].g;
            terrain_model_[index].b = height_map_[index4].b;
            terrain_model_[index].tu2 = tu2right;
            terrain_model_[index].tv2 = tv2bottom;
            index++;

            tu2left += increment_size;
            tu2right += increment_size;

            if(tu2right > 1.0f)
            {
                tu2left = 0.0f;
                tu2right = increment_size;
            }
        }

        tv2top += increment_size;
        tv2bottom += increment_size;
        if(tv2bottom > 1.0f)
        {
            tv2top = 0.0f;
            tv2bottom = increment_size;
        }
    }

    return true;
}

void TerrainClass::Shutdown_terrain_model()
{
    if(terrain_model_)
    {
        delete[] terrain_model_;
        terrain_model_ = nullptr;
    }
}

void TerrainClass::Calculate_terrain_vectors()
{
    int face_count, i, index;
    TEMP_VERTEX_TYPE vertex1, vertex2, vertex3;
    VECTOR_TYPE tangent, binormal;

    face_count = vertex_count_ / 3;
    index = 0;

    for (i = 0; i < face_count; i++)
    {
        vertex1.x = terrain_model_[index].x;
        vertex1.y = terrain_model_[index].y;
        vertex1.z = terrain_model_[index].z;
        vertex1.tu = terrain_model_[index].tu;
        vertex1.tv = terrain_model_[index].tv;
        vertex1.nx = terrain_model_[index].nx;
        vertex1.ny = terrain_model_[index].ny;
        vertex1.nz = terrain_model_[index].nz;
        index++;

        vertex2.x = terrain_model_[index].x;
        vertex2.y = terrain_model_[index].y;
        vertex2.z = terrain_model_[index].z;
        vertex2.tu = terrain_model_[index].tu;
        vertex2.tv = terrain_model_[index].tv;
        vertex2.nx = terrain_model_[index].nx;
        vertex2.ny = terrain_model_[index].ny;
        vertex2.nz = terrain_model_[index].nz;
        index++;

        vertex3.x = terrain_model_[index].x;
        vertex3.y = terrain_model_[index].y;
        vertex3.z = terrain_model_[index].z;
        vertex3.tu = terrain_model_[index].tu;
        vertex3.tv = terrain_model_[index].tv;
        vertex3.nx = terrain_model_[index].nx;
        vertex3.ny = terrain_model_[index].ny;
        vertex3.nz = terrain_model_[index].nz;
        index++;

        Calculate_tangent_binormal(vertex1, vertex2, vertex3, tangent, binormal);

        terrain_model_[index - 1].tx = tangent.x;
        terrain_model_[index - 1].ty = tangent.y;
        terrain_model_[index - 1].tz = tangent.z;
        terrain_model_[index - 1].bx = binormal.x;
        terrain_model_[index - 1].by = binormal.y;
        terrain_model_[index - 1].bz = binormal.z;

        terrain_model_[index - 2].tx = tangent.x;
        terrain_model_[index - 2].ty = tangent.y;
        terrain_model_[index - 2].tz = tangent.z;
        terrain_model_[index - 2].bx = binormal.x;
        terrain_model_[index - 2].by = binormal.y;
        terrain_model_[index - 2].bz = binormal.z;

        terrain_model_[index - 3].tx = tangent.x;
        terrain_model_[index - 3].ty = tangent.y;
        terrain_model_[index - 3].tz = tangent.z;
        terrain_model_[index - 3].bx = binormal.x;
        terrain_model_[index - 3].by = binormal.y;
        terrain_model_[index - 3].bz = binormal.z;
    }
}

void TerrainClass::Calculate_tangent_binormal(TEMP_VERTEX_TYPE vertex1, TEMP_VERTEX_TYPE vertex2, TEMP_VERTEX_TYPE vertex3, VECTOR_TYPE& tangent, VECTOR_TYPE& binormal)
{
    float vector1[3], vector2[3];
    float tu_vector[2], tv_vector[2];
    float den;
    float length;

    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;

    tu_vector[0] = vertex2.tu - vertex1.tu;
    tv_vector[0] = vertex2.tv - vertex1.tv;

    tu_vector[1] = vertex3.tu - vertex1.tu;
    tv_vector[1] = vertex3.tv - vertex1.tv;

    den = 1.0f / (tu_vector[0] * tv_vector[1] - tu_vector[1] * tv_vector[0]);

    tangent.x = (tv_vector[1] * vector1[0] - tv_vector[0] * vector2[0]) * den;
    tangent.y = (tv_vector[1] * vector1[1] - tv_vector[0] * vector2[1]) * den;
    tangent.z = (tv_vector[1] * vector1[2] - tv_vector[0] * vector2[2]) * den;

    binormal.x = (tu_vector[0] * vector2[0] - tu_vector[1] * vector1[0]) * den;
    binormal.y = (tu_vector[0] * vector2[1] - tu_vector[1] * vector1[1]) * den;
    binormal.x = (tu_vector[0] * vector2[2] - tu_vector[1] * vector1[2]) * den;

    length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;
}

bool TerrainClass::Check_height_of_triangle(float x, float z, float& height, float v0[3], float v1[3], float v2[3])
{
    float start_vector[3], direction_vector[3], edge1[3], edge2[3], normal[3];
    float Q[3], e1[3], e2[3], e3[3], edge_normal[3], temp[3];
    float magnitude, D, denominator, numerator, t, determinant;

    start_vector[0] = x;
    start_vector[1] = 0.0f;
    start_vector[2] = z;

    direction_vector[0] = 0.0f;
    direction_vector[1] = -1.0f;
    direction_vector[2] = 0.0f;

    edge1[0] = v1[0] - v0[0];
    edge1[1] = v1[1] - v0[1];
    edge1[2] = v1[2] - v0[2];

    edge2[0] = v2[0] - v0[0];
    edge2[1] = v2[1] - v0[1];
    edge2[2] = v2[2] - v0[2];

    //normal of triangle from two edges
    normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
    normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
    normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

    magnitude = (float)sqrt((normal[0] * normal[0]) + (normal[1] * normal[1]) + (normal[2] * normal[2]));
    normal[0] /= magnitude;
    normal[1] /= magnitude;
    normal[2] /= magnitude;

    //distance from origin to plane
    D = ((-normal[0] * v0[0]) + (-normal[1] * v0[1]) + (-normal[2] * v0[2]));
    denominator = ((normal[0] * direction_vector[0]) + (normal[1] * direction_vector[1]) + (normal[2] * direction_vector[2]));
    if (fabs(denominator) < 0.0001f)
        return false;

    numerator = -1.0f * (((normal[0] * start_vector[0]) + (normal[1] * start_vector[1]) + (normal[2] * start_vector[2])) + D);
    t = numerator / denominator;

    //find intersection
    Q[0] = start_vector[0] + (direction_vector[0] * t);
    Q[1] = start_vector[1] + (direction_vector[1] * t);
    Q[2] = start_vector[2] + (direction_vector[2] * t);

    e1[0] = v1[0] - v0[0];
    e1[1] = v1[1] - v0[1];
    e1[2] = v1[2] - v0[2];

    e2[0] = v2[0] - v1[0];
    e2[1] = v2[1] - v1[1];
    e2[2] = v2[2] - v1[2];

    e3[0] = v0[0] - v2[0];
    e3[1] = v0[1] - v2[1];
    e3[2] = v0[2] - v2[2];

    //normal of first edge
    edge_normal[0] = (e1[1] * normal[2]) - (e1[2] * normal[1]);
    edge_normal[1] = (e1[2] * normal[0]) - (e1[0] * normal[2]);
    edge_normal[2] = (e1[0] * normal[1]) - (e1[1] * normal[0]);

    //determinant to see if it is inside/outside or directly on edge
    temp[0] = Q[0] - v0[0];
    temp[1] = Q[1] - v0[1];
    temp[2] = Q[2] - v0[2];
    determinant = ((edge_normal[0] * temp[0]) + (edge_normal[1] * temp[1]) + (edge_normal[2] * temp[2]));
   
    //if its outside
    if (determinant > 0.001f)
        return false;

    //normal of second edge
    edge_normal[0] = (e2[1] * normal[2]) - (e2[2] * normal[1]);
    edge_normal[1] = (e2[2] * normal[0]) - (e2[0] * normal[2]);
    edge_normal[2] = (e2[0] * normal[1]) - (e2[1] * normal[0]);

    //determinant to see if it is inside/outside or directly on edge
    temp[0] = Q[0] - v1[0];
    temp[1] = Q[1] - v1[1];
    temp[2] = Q[2] - v1[2];
    determinant = ((edge_normal[0] * temp[0]) + (edge_normal[1] * temp[1]) + (edge_normal[2] * temp[2]));

    //if its outside
    if (determinant > 0.001f)
        return false;

    //normal of third edge
    edge_normal[0] = (e3[1] * normal[2]) - (e3[2] * normal[1]);
    edge_normal[1] = (e3[2] * normal[0]) - (e3[0] * normal[2]);
    edge_normal[2] = (e3[0] * normal[1]) - (e3[1] * normal[0]);

    //determinant to see if it is inside/outside or directly on edge
    temp[0] = Q[0] - v2[0];
    temp[1] = Q[1] - v2[1];
    temp[2] = Q[2] - v2[2];
    determinant = ((edge_normal[0] * temp[0]) + (edge_normal[1] * temp[1]) + (edge_normal[2] * temp[2]));

    //if its outside
    if (determinant > 0.001f)
        return false;

    //lol
    height = Q[1];
    
    return true;
}

bool TerrainClass::Load_terrain_cells(ID3D11Device* device)
{
    int cell_height, cell_width, cell_row_count, i, j, index;
    bool result;

    cell_height = 33;
    cell_width = 33;

    cell_row_count = (terrain_width_ - 1) / (cell_width - 1);
    cell_count_ = cell_row_count * cell_row_count;

    terrain_cells_ = new TerrainCellClass[cell_count_];
    if(!terrain_cells_)
        return false;

    for (j = 0; j < cell_row_count; j++)
    {
        for (i = 0; i < cell_row_count; i++)
        {
            index = (cell_row_count * j) + i;
            result = terrain_cells_[index].Initialize(device, terrain_model_, i, j, cell_height, cell_width, terrain_width_);
            if (!result)
                return false;
        }
    }
    return true;
}

void TerrainClass::Shutdown_terrain_cells()
{
    int i;
    if(terrain_cells_)
    {
        for (i = 0; i < cell_count_; i++)
            terrain_cells_[i].Shutdown();
        delete[] terrain_cells_;
        terrain_cells_ = nullptr;
    }
}

bool TerrainClass::Render_cell(ID3D11DeviceContext* device_context, int cell_id, FrustumClass* frustum)
{
    float max_width, max_height, max_depth, min_width, min_height, min_depth;
    bool result;

    terrain_cells_[cell_id].Get_cell_dimensions(max_width, max_height, max_depth, min_width, min_height, min_depth);
    result = frustum->Check_rectangle2(max_width, max_height, max_depth, min_width, min_height, min_depth);
    if(!result)
    {
        cells_culled_++;
        return false;
    }

    terrain_cells_[cell_id].Render(device_context);
    render_count_ += (terrain_cells_[cell_id].Get_vertex_count() / 3);
    cells_drawn_++;

    return true;
}

void TerrainClass::Render_cell_lines(ID3D11DeviceContext* device_context, int cell_id)
{
    terrain_cells_[cell_id].Render_line_buffers(device_context);
}

int TerrainClass::Get_cell_index_count(int cell_id)
{
    return terrain_cells_[cell_id].Get_index_count();
}

int TerrainClass::Get_cell_lines_index_count(int cell_id)
{
    return terrain_cells_[cell_id].Get_line_buffers_index_count();
}

int TerrainClass::Get_cell_count()
{
    return cell_count_;
}

int TerrainClass::Get_render_count()
{
    return render_count_;
}

int TerrainClass::Get_cells_drawn()
{
    return cells_drawn_;
}

int TerrainClass::Get_cells_culled()
{
    return cells_culled_;
}

bool TerrainClass::Get_height_at_position(float input_x, float input_z, float& height)
{
    int i, cell_id, index;
    float vertex1[3], vertex2[3], vertex3[3];
    bool found_height;
    float max_width, max_height, max_depth, min_width, min_height, min_depth;

    cell_id = 0;
    for (i = 0; i < cell_count_; i++)
    {
        terrain_cells_[i].Get_cell_dimensions(max_width, max_height, max_depth, min_width, min_height, min_depth);
        if((input_x < max_width) && (input_x > min_width) && (input_z < max_depth) && (input_z > min_depth))
        {
            cell_id = i;
            i = cell_count_;
        }
    }

    for (i = 0; i < (terrain_cells_[cell_id].Get_vertex_count() / 3); i++)
    {
        index = i * 3;
        vertex1[0] = terrain_cells_[cell_id].vertex_list[index].x;
        vertex1[1] = terrain_cells_[cell_id].vertex_list[index].y;
        vertex1[2] = terrain_cells_[cell_id].vertex_list[index].z;
        index++;

        vertex2[0] = terrain_cells_[cell_id].vertex_list[index].x;
        vertex2[1] = terrain_cells_[cell_id].vertex_list[index].y;
        vertex2[2] = terrain_cells_[cell_id].vertex_list[index].z;
        index++;

        vertex3[0] = terrain_cells_[cell_id].vertex_list[index].x;
        vertex3[1] = terrain_cells_[cell_id].vertex_list[index].y;
        vertex3[2] = terrain_cells_[cell_id].vertex_list[index].z;
        found_height = Check_height_of_triangle(input_x, input_z, height, vertex1, vertex2, vertex3);
        if (found_height)
            return true;
    }
    return false;
}



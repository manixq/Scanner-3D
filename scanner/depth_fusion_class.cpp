#include  "headers/depth_fusion_class.h"

DepthFusionClass::DepthFusionClass()
{
    vertex_buffer_ = nullptr;
    index_buffer_ = nullptr;
    depth_filename_ = nullptr;
    color_map_filename_ = nullptr;
    depth_map_ = nullptr;
    model_ = nullptr;
}

DepthFusionClass::DepthFusionClass(const DepthFusionClass&)
{
}

DepthFusionClass::~DepthFusionClass()
{
}

bool DepthFusionClass::Initialize(ID3D11Device* device, char* depth_filename, char* color_filename)
{
    bool result;

    depth_filename_ = depth_filename;
    color_map_filename_ = color_filename;

    result = Load_bitmap_depth_map();
    if (!result)
        return false;

    Set_coordinates();

    result = Calculate_normals();
    if (!result)
        return false;

    result = Load_color_map();
    if (!result)
        return false;

    result = Build_model();
    if (!result)
        return false;

    Shutdown_depth_map();

    Calculate_vectors();
    
    result = Initialize_buffers(device);
    if (!result)
        return false;

    Shutdown_model();

    return true;
}

void DepthFusionClass::Shutdown()
{
    Shutdown_buffers();
    Shutdown_model();
    Shutdown_depth_map();
}

bool DepthFusionClass::Render(ID3D11DeviceContext* device_context)
{
    Render_buffers(device_context);
    return true;
}

bool DepthFusionClass::Load_bitmap_depth_map()
{
    int error, image_size, i, j, k, index;
    FILE* file_ptr;
    unsigned long long count;
    BITMAPFILEHEADER bitmap_file_header;
    BITMAPINFOHEADER bitmap_info_header;
    unsigned char* bitmap_image;
    unsigned char depth;

    //TODO: read width/height from file
    model_width_ = 450;
    model_height_ = 375;

    depth_map_ = new DEPTH_MAP_TYPE[model_width_* model_height_];
    if (!depth_map_)
        return false;

    error = fopen_s(&file_ptr, depth_filename_, "rb");
    if (error != 0)
        return false;

    count = fread(&bitmap_file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    count = fread(&bitmap_info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
    if (count != 1)
        return false;

    if ((bitmap_info_header.biHeight != model_height_) || (bitmap_info_header.biWidth != model_width_))
        return false;

    image_size = model_height_ * ((model_width_ * 3) + 1);

    bitmap_image = new unsigned char[image_size];
    if (!bitmap_image)
        return false;

    fseek(file_ptr, bitmap_file_header.bfOffBits, SEEK_SET);
    count = fread(bitmap_image, 1, image_size, file_ptr);
    if (count != image_size)
        return false;

    error = fclose(file_ptr);
    if (error != 0)
        return false;

    k = 0;
    for (j = 0; j < model_height_; j++)
    {
        for (i = 0; i < model_width_; i++)
        {
            index = (model_width_ * (model_height_ - 1 - j)) + i;
            depth = bitmap_image[k];
            depth_map_[index].z = (float)depth / 10;
            k += 3;
        }
        k++;
    }
    delete[] bitmap_image;
    bitmap_image = nullptr;
    
    return true;
}

bool DepthFusionClass::Load_color_map()
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

    if ((bitmap_info_header.biHeight != model_height_) || (bitmap_info_header.biWidth != model_width_))
        return false;

    //need to add extra coz 257x257 non-divide-able
    image_size = model_height_ * ((model_width_ * 3) + 1);

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
    for (j = 0; j < model_height_; j++)
    {
        for (i = 0; i < model_width_; i++)
        {
            index = (model_width_ * (model_height_ - 1 - j)) + i;
            depth_map_[index].b = (float)bitmap_image[k] / 255.0f;
            depth_map_[index].g = (float)bitmap_image[k + 1] / 255.0f;
            depth_map_[index].r = (float)bitmap_image[k + 2] / 255.0f;
            k += 3;
        }
        //its for compenaste extra byte at end of each line in non modulo 2 bitmaps
        k++;
    }
    delete[] bitmap_image;
    bitmap_image = nullptr;
    
    return true;
}

void DepthFusionClass::Shutdown_depth_map()
{
    if(depth_map_)
    {
        delete[] depth_map_;
        depth_map_ = nullptr;
    }
}

void DepthFusionClass::Set_coordinates()
{
    int i, j, index;

    for (j = 0; j < model_height_; j++)
    {
        for (i = 0; i < model_width_; i++)
        {
            index = (model_width_ * j) + i;

            depth_map_[index].x = -(float)i / 10;
            depth_map_[index].y = -(float)j / 10;
            depth_map_[index].y += 60;
            depth_map_[index].z /= 2;
            depth_map_[index].z -= 20;
        }
    }
}

bool DepthFusionClass::Calculate_normals()
{
    int i, j, index1, index2, index3, index;
    float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
    VECTOR_TYPE* normals;

    normals = new VECTOR_TYPE[(model_height_ - 1) * (model_width_ - 1)];
    if (!normals)
        return false;

    for (j = 0; j < (model_height_ - 1); j++)
    {
        for (i = 0; i < (model_width_ - 1); i++)
        {
            index1 = ((j + 1) * model_width_) + i;
            index2 = ((j + 1) * model_width_) + (i + 1);
            index3 = ((j)* model_width_) + i;

            vertex1[0] = depth_map_[index1].x;
            vertex1[1] = depth_map_[index1].y;
            vertex1[2] = depth_map_[index1].z;

            vertex2[0] = depth_map_[index2].x;
            vertex2[1] = depth_map_[index2].y;
            vertex2[2] = depth_map_[index2].z;

            vertex3[0] = depth_map_[index3].x;
            vertex3[1] = depth_map_[index3].y;
            vertex3[2] = depth_map_[index3].z;

            vector1[0] = vertex1[0] - vertex3[0];
            vector1[1] = vertex1[1] - vertex3[1];
            vector1[2] = vertex1[2] - vertex3[2];
            vector2[0] = vertex3[0] - vertex2[0];
            vector2[1] = vertex3[1] - vertex2[1];
            vector2[2] = vertex3[2] - vertex2[2];

            index = (j * (model_width_ - 1)) + i;

            normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
            normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
            normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

            length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) + (normals[index].z * normals[index].z));

            normals[index].x = (normals[index].x / length);
            normals[index].y = (normals[index].y / length);
            normals[index].z = (normals[index].z / length);
        }
    }

    for (j = 0; j < model_height_; j++)
    {
        for (i = 0; i < model_width_; i++)
        {
            sum[0] = 0.0f;
            sum[1] = 0.0f;
            sum[2] = 0.0f;

            if (((i - 1) >= 0) && ((j - 1) >= 0))
            {
                index = ((j - 1) * (model_width_ - 1)) + (i - 1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if ((i < (model_width_ - 1)) && ((j - 1) >= 0))
            {
                index = ((j - 1) * (model_width_ - 1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if (((i - 1) >= 0) && (j < model_height_ - 1))
            {
                index = (j * (model_width_ - 1)) + (i - 1);
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            if ((i < (model_width_ - 1)) && (j < model_height_ - 1))
            {
                index = (j * (model_width_ - 1)) + i;
                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
            }

            length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
            index = (j * model_width_) + i;

            //depth_map_[index].nx = (sum[0] / length);
            //depth_map_[index].ny = (sum[1] / length);
           // depth_map_[index].nz = (sum[2] / length);
        }
    }
    delete[] normals;
    normals = nullptr;

    return true;
}

bool DepthFusionClass::Build_model()
{
    int i, j, index, index1, index2, index3, index4;
    float increment_size_width, increment_size_height;
    
    vertex_count_ = (model_height_ - 1) * (model_width_ - 1) * 6;

    model_ = new MODEL_TYPE[vertex_count_];
    if (!model_)
        return false;    

    increment_size_width = 1 / model_width_;
    increment_size_height = 1 / model_height_;

    float tu2left = 0.0f;
    float tu2right = 1.0f;
    float tv2top = 0.0f;
    float tv2bottom = 1.0f;

    //triangle strip
    index = 0;
    for (j = 0; j < (model_height_ - 1); j++)
    {
        for (i = 0; i < (model_width_ - 1); i++)
        {
            index1 = (model_width_ * j) + i;
            index2 = (model_width_ * j) + (i + 1);
            index3 = (model_width_ * (j + 1)) + i;
            index4 = (model_width_ * (j + 1)) + (i + 1);

            model_[index].x = depth_map_[index1].x;
            model_[index].y = depth_map_[index1].y;
            model_[index].z = depth_map_[index1].z;
           // model_[index].tu = tu2left;
           // model_[index].tv = tv2top;
           // model_[index].nx = depth_map_[index1].nx;
           // model_[index].ny = depth_map_[index1].ny;
           // model_[index].nz = depth_map_[index1].nz;
            model_[index].r = depth_map_[index1].r;
            model_[index].g = depth_map_[index1].g;
            model_[index].b = depth_map_[index1].b;
            index++;

            model_[index].x = depth_map_[index2].x;
            model_[index].y = depth_map_[index2].y;
            model_[index].z = depth_map_[index2].z;
           // model_[index].tu = tu2right;
            //model_[index].tv = tv2top;
            //model_[index].nx = depth_map_[index2].nx;
            //model_[index].ny = depth_map_[index2].ny;
            //model_[index].nz = depth_map_[index2].nz;
            model_[index].r = depth_map_[index2].r;
            model_[index].g = depth_map_[index2].g;
            model_[index].b = depth_map_[index2].b;
            index++;

            model_[index].x = depth_map_[index3].x;
            model_[index].y = depth_map_[index3].y;
            model_[index].z = depth_map_[index3].z;
           // model_[index].tu = tu2left;
           // model_[index].tv = tv2bottom;
           // model_[index].nx = depth_map_[index3].nx;
          //  model_[index].ny = depth_map_[index3].ny;
          //  model_[index].nz = depth_map_[index3].nz;
            model_[index].r = depth_map_[index3].r;
            model_[index].g = depth_map_[index3].g;
            model_[index].b = depth_map_[index3].b;
            index++;

            model_[index].x = depth_map_[index3].x;
            model_[index].y = depth_map_[index3].y;
            model_[index].z = depth_map_[index3].z;
           // model_[index].tu = tu2left;
           // model_[index].tv = tv2bottom;
          //  model_[index].nx = depth_map_[index3].nx;
          //  model_[index].ny = depth_map_[index3].ny;
          //  model_[index].nz = depth_map_[index3].nz;
            model_[index].r = depth_map_[index3].r;
            model_[index].g = depth_map_[index3].g;
            model_[index].b = depth_map_[index3].b;
            index++;

            model_[index].x = depth_map_[index2].x;
            model_[index].y = depth_map_[index2].y;
            model_[index].z = depth_map_[index2].z;
          //  model_[index].tu = tu2right;
           // model_[index].tv = tv2top;
           // model_[index].nx = depth_map_[index2].nx;
           // model_[index].ny = depth_map_[index2].ny;
           // model_[index].nz = depth_map_[index2].nz;
            model_[index].r = depth_map_[index2].r;
            model_[index].g = depth_map_[index2].g;
            model_[index].b = depth_map_[index2].b;
            index++;

            model_[index].x = depth_map_[index4].x;
            model_[index].y = depth_map_[index4].y;
            model_[index].z = depth_map_[index4].z;
           // model_[index].tu = tu2right;
          //  model_[index].tv = tv2bottom;
          //  model_[index].nx = depth_map_[index4].nx;
         //   model_[index].ny = depth_map_[index4].ny;
         //   model_[index].nz = depth_map_[index4].nz;
            model_[index].r = depth_map_[index4].r;
            model_[index].g = depth_map_[index4].g;
            model_[index].b = depth_map_[index4].b;
            index++;

         
        }
        
    }
    return true;
}

void DepthFusionClass::Shutdown_model()
{
    if (model_)
    {
        delete[] model_;
        model_ = nullptr;
    }
}

//TODO: future
void DepthFusionClass::Calculate_vectors()
{
}

void DepthFusionClass::Render_buffers(ID3D11DeviceContext* device_context)
{
    unsigned int stride;
    unsigned int offset;

    stride = sizeof(VERTEX_TYPE);
    offset = 0;
    device_context->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
    device_context->IASetIndexBuffer(index_buffer_, DXGI_FORMAT_R32_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool DepthFusionClass::Initialize_buffers(ID3D11Device* device)
{
    VERTEX_TYPE* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;
    int i;

    vertex_count_ = (model_width_ - 1) * (model_height_ - 1) * 6;
    index_count_ = vertex_count_;

    vertices = new VERTEX_TYPE[vertex_count_];
    if (!vertices)
        return false;

    indices = new unsigned long[index_count_];
    if (!indices)
        return false;

    for (i = 0; i < vertex_count_; i++)
    {
        vertices[i].position = XMFLOAT3(model_[i].x, model_[i].y, model_[i].z);
        vertices[i].color = XMFLOAT3(model_[i].r, model_[i].g, model_[i].b);
        //vertices[i].texture = XMFLOAT2(model_[i].tu, model_[i].tv);
        indices[i] = i;
    }

    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = sizeof(VERTEX_TYPE) * vertex_count_;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    vertex_data.pSysMem = vertices;
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertex_buffer_);
    if (FAILED(result))
        return false;

    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = sizeof(unsigned long)*index_count_;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    index_data.pSysMem = indices;
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&index_buffer_desc, &index_data, &index_buffer_);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = nullptr;
    delete[]indices;
    indices = nullptr;

    return true;
}

void DepthFusionClass::Shutdown_buffers()
{
    if(index_buffer_)
    {
        index_buffer_->Release();
        index_buffer_ = nullptr;
    }

    if(vertex_buffer_)
    {
        vertex_buffer_->Release();
        vertex_buffer_ = nullptr;
    }
}

int DepthFusionClass::Get_index_count()
{
    return index_count_;
}

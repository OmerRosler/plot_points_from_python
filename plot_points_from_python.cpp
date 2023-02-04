// plot_points_from_python.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>

#include "bitmap_image.hpp"
#include "utils.hpp"

namespace frc
{
struct image_data_t : image_metadata_t
{
    bitmap_image img;
};

}

class reader_state_machine
{
public:
    reader_state_machine(frc::image_data_t& img):
        m_img(img),
        m_polynomial(),
        m_color()
    {}

    void read_poly(std::wstring m_polynomial)
    {
        //TODO: Input this number from the python program
        constexpr auto num_of_polys = 17;
        static auto color_ind = 0;
        color_ind++;
        constexpr int jump_size = 255 / (num_of_polys + 1);
        assert(color_ind <= num_of_polys);
        auto color_v = jump_size * color_ind;
        m_color = rgb_t{ 
            unsigned char (color_v/2), 
            unsigned char(color_v / 7), 
            unsigned char(color_v / 11) };
    }

    void read_k(int k)
    {
        m_k = k;
    }
    void set_pixel_of_point(double x, double y)
    {
        auto [pixel_x, pixel_y] = m_img.pixel_id_from_value(x, y);
        m_img.img.set_pixel(pixel_x, pixel_y, m_color);
    }
private:
    frc::image_data_t& m_img;
    std::string m_polynomial;
    int m_k;
    rgb_t m_color;
};
std::wistream& read_file(std::wistream& is, frc::image_data_t& img)
{
    std::wstring line;
    reader_state_machine sm(img);
    while (std::getline(is, line))
    {
        if (line == L"STOP")
        {
            // Read the new polynomial
            std::getline(is, line);
            sm.read_poly(line);
            // Read the `k` value used for the trap
            std::getline(is, line);
            std::wstringstream line_stream(line);
            int k;
            line_stream >> k;
            sm.read_k(k);
            // Start reading points
            continue;
        }
        std::wstringstream line_stream(line);
        double x, y;
        line_stream >> x >> y;
        sm.set_pixel_of_point(x, y);
    }
    return is;
        
}


int main()
{
    std::string py_out_file = "py_out.txt";
    std::wifstream infile(py_out_file);

    frc::image_data_t image{
        {.res = {600, 600},
        .dom = {.x{0.5,1}, .y{0.5,1}}
        },
        bitmap_image("input_image.bmp"),
    };

    read_file(infile, image);
    
    image.img.save_image("written_image.bmp");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef OPTICAL_SYSTEM_H
#define OPTICAL_SYSTEM_H

namespace lf
{

struct optical_system
{
public:
    unsigned blades; //<< Number of aperture blades (at least 3)
    float f_number; //<< f-number of the camera (from 1 to +inf)
    float sensor_size; //<< Size of the sensor, from > 0 to 1
    float focal_length; //<< Focal length of the lens, in mm
    
    // other stuff
    
    std::string name; //<< Name of this optical system

    //<< Loads an optical system from a file
    optical_system(const std::string& path);

private:
    optical_system() = delete;
};

class 
{

};

};

#endif

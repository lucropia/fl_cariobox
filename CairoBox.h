/* --------------------------------------------------------------------------
 * FileName: CairoBox.h
 * Author: David M. Allen
 *
 * Emeritus Professor
 * Department of Statistics
 * 325B Multidisciplinary Science Building
 * University of Kentucky
 * Lexington, Kentucky 40536-0082
 *
 * Email: david.allen@uky.edu
 * Phone: 859 257-6901
 *
 * CopyRight: Public Domain.
 * ------------------------------------------------------------------------ */

#ifndef __CAIROBOX
#define __CAIROBOX

#include <FL/Fl_Box.H>
#include <cairo/cairo.h>

#ifdef WIN32
#  include <cairo-win32.h>
#elif defined __APPLE__
#  include <cairo-quartz.h>
#elif defined linux
#  include <cairo/cairo-xlib.h>
#else
#error "This platform isnot supported"
#endif

class CairoBox : public Fl_Box
{
protected:
    void draw(void);
    cairo_t*          cr;
    cairo_surface_t*  surface;
    cairo_surface_t*  base_surface;
    cairo_surface_t*  set_surface(int wo, int ho);
public:
    virtual void graphic(cairo_t* cr, double, double, double, double) = 0;
    void out_png(const char* filename, int, int);
    void out_svg(const char* filename, int, int);
    void out_eps(const char* filename, int, int);
    void out_pdf(const char* filename, int, int);
    CairoBox(int x, int y, int w, int h);
};

#endif /* __CAIROBOX */


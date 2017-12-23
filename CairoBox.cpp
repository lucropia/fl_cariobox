/* --------------------------------------------------------------------------
 * FileName: CairoBox.cpp
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


#include <cairo/cairo-svg.h>
#include <cairo/cairo-ps.h>
#include <cairo/cairo-pdf.h>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif // M$ Windows only.

#include <FL/fl_draw.H>

#include "CairoBox.h"

CairoBox::CairoBox(int x, int y, int w, int h) : Fl_Box(x,y,w,h)
{
    surface      = NULL;
    cr           = NULL;
#ifdef linux
    base_surface = NULL;
#endif
}

cairo_surface_t*  CairoBox::set_surface(int wo, int ho)
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#warning "WIN32 mode"
    /* Get a Cairo surface for the current DC */
    //HDC dc = fl_gc;                                     /* Exported by fltk */
    HDC dc = GetWindowDC(fl_window);
    base_surface = cairo_win32_surface_create(dc);
    return cairo_surface_create_for_rectangle(base_surface, x() + 3, y()+ 25, wo, ho);
#elif defined (__APPLE__)
#warning "Apple Quartz mode"
    /*
     * Get a Cairo surface for the current CG context
     *
     * modify by Phil Zhang(Mar. 18, 2017):
     * in next two lines, delete the temple CGContext, and add TranslaiteCTM
     * to make the view window set to current CairoBox widget.
     */
    //CGContextTranslateCTM (fl_gc, x(), y());
    //return cairo_quartz_surface_create_for_cg_context(fl_gc, wo, ho);
    base_surface = cairo_quartz_surface_create_for_cg_context(fl_gc, x() + wo, y() + ho);
#elif defined (linux)
#warning "Linux XLib mode"
    /*
     * Get a Cairo surface for the current display
     *
     * modify by Phil Zhang(Apr. 22, 2017):
     * create a surface of rectangle to fit in CairoBox location.
     */
    base_surface = cairo_xlib_surface_create(fl_display, fl_window, fl_visual->visual, wo + x(), ho + y());
#else
#error "This platform isnot supported."
#endif
    return cairo_surface_create_for_rectangle(base_surface, x(), y(), wo, ho);
}

void CairoBox::out_png(const char* filename, int wpix, int hpix)
{
    cairo_surface_t* surface;
    cairo_t*         cr;

    // setup, see cairo_format_t for discussion of first argument
    surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, wpix, hpix);
    cr      = cairo_create (surface);

    cairo_translate(cr, 0.5, 0.5);       // for anti-aliasing

    // virtual function
    graphic(cr, 0, 0, wpix, hpix);

    cairo_surface_write_to_png (surface, filename);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
#ifdef linux
    cairo_surface_destroy(base_surface);
#endif
}

void CairoBox::out_svg(const char* filename, int wpts, int hpts)
{
    cairo_surface_t* surface;
    cairo_t*         cr;

    surface = cairo_svg_surface_create (filename, wpts, hpts);
    cr      = cairo_create (surface);
    cairo_set_source_rgb (cr, 0, 0, 0);

    // user coordinates -- see cairo_svg_surface_create
    graphic(cr, 0, 0, wpts, hpts);

    cairo_destroy (cr);
    cairo_surface_destroy (surface);
#ifdef linux
    cairo_surface_destroy(base_surface);
#endif
}

void CairoBox::out_eps(const char* filename, int wpts, int hpts)
{
    cairo_surface_t* surface;
    cairo_t*         cr;

    surface = cairo_ps_surface_create (filename, wpts, hpts);
    cairo_ps_surface_set_eps(surface, 1);
    cr      = cairo_create (surface);
    cairo_set_source_rgb (cr, 0, 0, 0);

    // user coordinates -- see cairo_ps_surface_create
    graphic(cr, 0, 0, wpts, hpts);

    cairo_show_page (cr);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
#ifdef linux
    cairo_surface_destroy(base_surface);
#endif
    return;
}

void CairoBox::out_pdf(const char* filename, int wpts, int hpts)
{
    cairo_surface_t* surface;
    cairo_t*         cr;

    surface = cairo_pdf_surface_create(filename, wpts, hpts);
    cr      = cairo_create (surface);
    cairo_set_source_rgb (cr, 0, 0, 0);

    // user coordinates -- see cairo_pdf_surface_create
    graphic(cr, 0, 0, wpts, hpts);

    cairo_show_page(cr);
    cairo_destroy (cr);
    cairo_surface_destroy(surface);
    cairo_surface_destroy(base_surface);
    return;
}

void CairoBox::draw(void)
{
    // using fltk functions, set up white background with thin black frame
    fl_push_no_clip();            /* remove any clipping region set by the expose events... */
    fl_push_clip(x(), y(), w(), h());
    fl_color(FL_WHITE);
    fl_rectf(x(), y(), w(), h());
    fl_color(FL_BLACK);
    fl_rect(x(), y(), w(), h());

    // set up cairo structures
    surface = set_surface(w(), h());
    cr      = cairo_create(surface);

    /* All Cairo co-ordinates are shifted by 0.5 pixels to correct anti-aliasing */
    cairo_translate(cr, 0.5, 0.5);

    // virtual function defined in driver program
    graphic(cr, x(), y(), w(), h());

    // release the cairo context
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    cairo_surface_destroy(base_surface);

    // remove clip regions
    fl_pop_clip();                          // local clip region
    fl_pop_clip();                          // "no_clip" region
}


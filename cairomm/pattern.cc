/* Copyright (C) 2005 The cairomm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <cairomm/pattern.h>
#include <cairomm/private.h>
#include <cairomm/matrix.h>

namespace Cairo
{

Pattern::Pattern()
: m_cobject(nullptr)
{
}

Pattern::Pattern(cairo_pattern_t* cobject, bool has_reference)
: m_cobject(nullptr)
{
  if(has_reference)
    m_cobject = cobject;
  else
    m_cobject = cairo_pattern_reference(cobject);
}

Pattern::~Pattern()
{
  if(m_cobject)
    cairo_pattern_destroy(m_cobject);
}

void Pattern::reference() const
{
 cairo_pattern_reference(m_cobject);
}

void Pattern::unreference() const
{
  cairo_pattern_destroy(m_cobject);
}

void Pattern::set_matrix(const Matrix& matrix)
{
  cairo_pattern_set_matrix(m_cobject, (cairo_matrix_t*)&matrix);
  check_object_status_and_throw_exception(*this);
}

void Pattern::get_matrix(Matrix& matrix) const
{
  cairo_pattern_get_matrix(m_cobject, (cairo_matrix_t*)&matrix);
  check_object_status_and_throw_exception(*this);
}

void Pattern::set_matrix(const cairo_matrix_t& matrix)
{
  cairo_pattern_set_matrix(m_cobject, (cairo_matrix_t*)&matrix);
  check_object_status_and_throw_exception(*this);
}

void Pattern::get_matrix(cairo_matrix_t& matrix) const
{
  cairo_pattern_get_matrix(m_cobject, (cairo_matrix_t*)&matrix);
  check_object_status_and_throw_exception(*this);
}

Matrix Pattern::get_matrix() const
{
  Cairo::Matrix m;
  cairo_pattern_get_matrix(m_cobject, (cairo_matrix_t*)&m);
  check_object_status_and_throw_exception(*this);
  return m;
}

PatternType Pattern::get_type() const
{
  auto pattern_type = cairo_pattern_get_type(m_cobject);
  check_object_status_and_throw_exception(*this);
  return static_cast<PatternType>(pattern_type);
}

void Pattern::set_extend(Extend extend)
{
  cairo_pattern_set_extend(m_cobject, (cairo_extend_t)extend);
  check_object_status_and_throw_exception(*this);
}

Extend Pattern::get_extend() const
{
  const auto result = static_cast<Extend>(cairo_pattern_get_extend(m_cobject));
  check_object_status_and_throw_exception(*this);
  return result;
}

SolidPattern::SolidPattern(cairo_pattern_t* cobject, bool has_reference)
: Pattern(cobject, has_reference)
{
}
void
SolidPattern::get_rgba(double& red, double& green,
                        double& blue, double& alpha) const
{
  // ignore the return value since we know that this is a solid color pattern
  cairo_pattern_get_rgba(m_cobject, &red, &green, &blue, &alpha);
  check_object_status_and_throw_exception(*this);
}

SolidPattern::~SolidPattern()
{
}

RefPtr<SolidPattern> SolidPattern::create_rgb(double red, double green, double blue)
{
  auto cobject = cairo_pattern_create_rgb(red, green, blue);
  check_status_and_throw_exception(cairo_pattern_status(cobject)); 
  return RefPtr<SolidPattern>(new SolidPattern(cobject, true /* has reference */));
}

RefPtr<SolidPattern> SolidPattern::create_rgba(double red, double green, double blue, double alpha)
{
  cairo_pattern_t* cobject  = cairo_pattern_create_rgba(red, green, blue, alpha);
  check_status_and_throw_exception(cairo_pattern_status(cobject));
  return RefPtr<SolidPattern>(new SolidPattern(cobject, true /* has reference */));
}


SurfacePattern::SurfacePattern(const RefPtr<Surface>& surface)
{
  m_cobject = cairo_pattern_create_for_surface(surface->cobj());
  check_object_status_and_throw_exception(*this); 
}

RefPtr<Surface>
SurfacePattern::get_surface()
{
  cairo_surface_t* surface = nullptr;
  // we can ignore the return value since we know this is a surface pattern
  cairo_pattern_get_surface(const_cast<cairo_pattern_t*>(m_cobject), &surface);
  check_object_status_and_throw_exception(*this);
  return RefPtr<Surface>(new Surface(surface, false /* does not have reference */));
}

RefPtr<const Surface>
SurfacePattern::get_surface() const
{
  return const_cast<SurfacePattern*>(this)->get_surface();
}

RefPtr<SurfacePattern> SurfacePattern::create(const RefPtr<Surface>& surface)
{
  return RefPtr<SurfacePattern>(new SurfacePattern(surface));
}

SurfacePattern::SurfacePattern(cairo_pattern_t* cobject, bool has_reference)
: Pattern(cobject, has_reference)
{
}

SurfacePattern::~SurfacePattern()
{
}

#ifndef CAIROMM_DISABLE_DEPRECATED
void SurfacePattern::set_extend(Extend extend)
{
  Pattern::set_extend(extend);
}

Extend SurfacePattern::get_extend() const
{
  return Pattern::get_extend();
}
#endif //CAIROMM_DISABLE_DEPRECATED

void SurfacePattern::set_filter(Filter filter)
{
  cairo_pattern_set_filter(m_cobject, (cairo_filter_t)filter);
  check_object_status_and_throw_exception(*this);
}

Filter SurfacePattern::get_filter() const
{
  auto result = static_cast<Filter>(cairo_pattern_get_filter(m_cobject));
  check_object_status_and_throw_exception(*this);
  return result;
}



Gradient::Gradient()
{
}

Gradient::Gradient(cairo_pattern_t* cobject, bool has_reference)
: Pattern(cobject, has_reference)
{
}

Gradient::~Gradient()
{
}

void Gradient::add_color_stop_rgb(double offset, double red, double green, double blue)
{
  cairo_pattern_add_color_stop_rgb(m_cobject, offset, red, green, blue);
  check_object_status_and_throw_exception(*this);
}

void Gradient::add_color_stop_rgba(double offset, double red, double green, double blue, double alpha)
{
  cairo_pattern_add_color_stop_rgba(m_cobject, offset, red, green, blue, alpha);
  check_object_status_and_throw_exception(*this);
}

std::vector<ColorStop>
Gradient::get_color_stops() const
{
  std::vector<ColorStop> stops;

  int num_stops = 0;
  // we can ignore the return value since we know this is a gradient pattern
  cairo_pattern_get_color_stop_count(m_cobject, &num_stops);
  // since we know the total number of stops, we can avoid re-allocation with
  // each addition to the vector by pre-allocating the required number
  stops.reserve(num_stops);
  for(int i = 0; i < num_stops; ++i)
  {
    ColorStop stop;
    cairo_pattern_get_color_stop_rgba(m_cobject, i, &stop.offset, &stop.red,
                                      &stop.green, &stop.blue, &stop.alpha);
    stops.push_back(stop);
  }
  return stops;
}


LinearGradient::LinearGradient(double x0, double y0, double x1, double y1)
{
  m_cobject = cairo_pattern_create_linear(x0, y0, x1, y1);
  check_object_status_and_throw_exception(*this); 
}

void
LinearGradient::get_linear_points(double &x0, double &y0,
                                   double &x1, double &y1) const
{
  // ignore the return value since we know that this is a linear gradient
  // pattern
  cairo_pattern_get_linear_points(m_cobject, &x0, &y0, &x1, &y1);
  check_object_status_and_throw_exception(*this);
}


RefPtr<LinearGradient> LinearGradient::create(double x0, double y0, double x1, double y1)
{
  return RefPtr<LinearGradient>(new LinearGradient(x0, y0, x1, y1));
}

LinearGradient::LinearGradient(cairo_pattern_t* cobject, bool has_reference)
: Gradient(cobject, has_reference)
{
}

LinearGradient::~LinearGradient()
{
}


RadialGradient::RadialGradient(double cx0, double cy0, double radius0, double cx1, double cy1, double radius1)
{
  m_cobject = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
  check_object_status_and_throw_exception(*this); 
}

void
RadialGradient::get_radial_circles(double& x0, double& y0, double& r0,
                                    double& x1, double& y1, double& r1) const
{
  // ignore the return value since we know that this is a radial gradient
  // pattern
  cairo_pattern_get_radial_circles(const_cast<cairo_pattern_t*>(m_cobject),
                                    &x0, &y0, &r0, &x1, &y1, &r1);
  check_object_status_and_throw_exception(*this); 
}


RefPtr<RadialGradient> RadialGradient::create(double cx0, double cy0, double radius0, double cx1, double cy1, double radius1)
{
  return RefPtr<RadialGradient>(new RadialGradient(cx0, cy0, radius0, cx1, cy1, radius1));
}

RadialGradient::RadialGradient(cairo_pattern_t* cobject, bool has_reference)
: Gradient(cobject, has_reference)
{
}

RadialGradient::~RadialGradient()
{
}


} //namespace Cairo

// vim: ts=2 sw=2 et

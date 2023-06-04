// falaise/snemo/geometry/two_arms_curve_3d.cc
/*
 * Copyright (C) 2021 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library
#include <cmath>

// Ourselves:
#include <falaise/snemo/geometry/two_arms_curve_3d.h>

// - Bayeux
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace geometry {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(two_arms_curve_3d,
                                                      "snemo::geometry::two_arms_curve_3d")

    // static
    const std::string & two_arms_curve_3d::two_arms_label()
    {
      static const std::string label{"two_arms_curve_3d"};
      return label;
    }

    // override
    unsigned int two_arms_curve_3d::get_number_of_paths() const
    {
      return 2;
    }

    // static
    std::string two_arms_curve_3d::to_string(shape_code_type code_)
    {
      if (code_ == SHAPE_CODE_LINE) return std::string("line");
      if (code_ == SHAPE_CODE_HELIX) return std::string("helix");
      return std::string("invalid");
    }

    bool two_arms_curve_3d::is_valid() const
    {
      if (_first_arm_.code == SHAPE_CODE_INVALID) return false;
      if (_first_arm_.code == SHAPE_CODE_LINE) return _first_arm_.line.is_valid();
      if (_first_arm_.code == SHAPE_CODE_HELIX) return _first_arm_.helix.is_valid();
      if (_second_arm_.code == SHAPE_CODE_INVALID) return false;
      if (_second_arm_.code == SHAPE_CODE_LINE) return _second_arm_.line.is_valid();
      if (_second_arm_.code == SHAPE_CODE_HELIX) return _second_arm_.helix.is_valid();
      return true;
    }

    two_arms_curve_3d::two_arms_curve_3d()
      : geomtools::i_shape_1d()
    {
    }

    two_arms_curve_3d::two_arms_curve_3d(double tolerance_)
       : geomtools::i_shape_1d(tolerance_)
    {
    }
  
    two_arms_curve_3d::~two_arms_curve_3d()
    {
    }

    void two_arms_curve_3d::reset_arms()
    {
      if (_second_arm_.code == SHAPE_CODE_HELIX) _second_arm_.helix = geomtools::helix_3d{};
      if (_second_arm_.code == SHAPE_CODE_LINE) _second_arm_.line = geomtools::line_3d{};
      _second_arm_.code = SHAPE_CODE_INVALID;
      if (_first_arm_.code == SHAPE_CODE_HELIX) _first_arm_.helix = geomtools::helix_3d{};
      if (_first_arm_.code == SHAPE_CODE_LINE) _first_arm_.line = geomtools::line_3d{};
      _first_arm_.code = SHAPE_CODE_INVALID;
      return;
    }
    
    void two_arms_curve_3d::set_first_arm(const geomtools::line_3d & line_)
    {
      _first_arm_.line = line_;
      _first_arm_.code = SHAPE_CODE_LINE;
      _second_arm_.code = SHAPE_CODE_INVALID;
      _second_arm_.helix = geomtools::helix_3d{};
      return;
    }

    void two_arms_curve_3d::set_first_arm(const geomtools::helix_3d & helix_)
    {
      _first_arm_.helix = helix_;
      _first_arm_.code = SHAPE_CODE_HELIX;
      _second_arm_.code = SHAPE_CODE_INVALID;
      _second_arm_.line = geomtools::line_3d{};
      return;
    }

    bool two_arms_curve_3d::can_second_arm(const geomtools::line_3d & line_) const
    {
      if (_first_arm_.code == SHAPE_CODE_INVALID) {
        DT_THROW(std::logic_error, "First arm is not set yet!");        
      }
      geomtools::vector_3d last_at_first;
      if (_first_arm_.code == SHAPE_CODE_LINE) {
        last_at_first = _first_arm_.line.get_last();
      } else if (_first_arm_.code == SHAPE_CODE_HELIX) {
        last_at_first = _first_arm_.helix.get_last();
      }
      return (line_.get_first() - last_at_first).mag() <= get_tolerance();
    }

    void two_arms_curve_3d::set_second_arm(const geomtools::line_3d & line_)
    {
      if (_first_arm_.code == SHAPE_CODE_INVALID) {
        DT_THROW(std::logic_error, "First arm is not set yet!");        
      }
      geomtools::vector_3d last_at_first;
      if (_first_arm_.code == SHAPE_CODE_LINE) {
        last_at_first = _first_arm_.line.get_last();
      } else if (_first_arm_.code == SHAPE_CODE_HELIX) {
        last_at_first = _first_arm_.helix.get_last();
      }
      DT_THROW_IF((line_.get_first() - last_at_first).mag() > get_tolerance(),
                  std::logic_error,
                  "Second line arm (first=" << geomtools::to_xyz(line_.get_first())
                  << ") cannot connect the first arm (last=" << geomtools::to_xyz(last_at_first)
                  << ") at requested tolerance!");
      _second_arm_.line = line_;
      _second_arm_.code = SHAPE_CODE_LINE;
      return;
    }
       
    bool two_arms_curve_3d::can_second_arm(const geomtools::helix_3d & helix_) const
    {
      if (_first_arm_.code == SHAPE_CODE_INVALID) {
        DT_THROW(std::logic_error, "First arm is not set yet!");        
      }
      geomtools::vector_3d last_at_first;
      if (_first_arm_.code == SHAPE_CODE_LINE) {
        last_at_first = _first_arm_.line.get_last();
      } else if (_first_arm_.code == SHAPE_CODE_HELIX) {
        last_at_first = _first_arm_.helix.get_last(); 
      }
      return (helix_.get_first() - last_at_first).mag() <= get_tolerance();
    }
        
    void two_arms_curve_3d::set_second_arm(const geomtools::helix_3d & helix_)
    {
      if (_first_arm_.code == SHAPE_CODE_INVALID) {
        DT_THROW(std::logic_error, "First arm is not set yet!");        
      }
      geomtools::vector_3d last_at_first;
      if (_first_arm_.code == SHAPE_CODE_LINE) {
        last_at_first = _first_arm_.line.get_last();
      } else if (_first_arm_.code == SHAPE_CODE_HELIX) {
        last_at_first = _first_arm_.helix.get_last(); 
      }
      DT_THROW_IF((helix_.get_first() - last_at_first).mag() > get_tolerance(),
                  std::logic_error,
                  "Second helix arm (first=" << geomtools::to_xyz(helix_.get_first())
                  << ") cannot connect the first arm (last=" << geomtools::to_xyz(last_at_first)
                  << ") at requested tolerance!");
      _second_arm_.helix = helix_;
      _second_arm_.code = SHAPE_CODE_HELIX;
      return;
    }

    bool two_arms_curve_3d::is_first_arm_line() const
    {
      return _first_arm_.code == SHAPE_CODE_LINE;
    }

    bool two_arms_curve_3d::is_first_arm_helix() const
    {
      return _first_arm_.code == SHAPE_CODE_HELIX;
    }

    bool two_arms_curve_3d::is_second_arm_line() const
    {
      return _second_arm_.code == SHAPE_CODE_LINE;
    }

    bool two_arms_curve_3d::is_second_arm_helix() const
    {
      return _second_arm_.code == SHAPE_CODE_HELIX;
    }

    geomtools::vector_3d two_arms_curve_3d::get_first_point() const
    {
      DT_THROW_IF(_first_arm_.code == SHAPE_CODE_INVALID, std::logic_error, "Invalid first arm!");
      if (is_first_arm_line()) {
        return _first_arm_.line.get_first();
      }
      return _first_arm_.helix.get_first();
    }
    
    geomtools::vector_3d two_arms_curve_3d::get_last_point() const
    {
      DT_THROW_IF(_second_arm_.code == SHAPE_CODE_INVALID, std::logic_error, "Invalid second arm!");
      if (is_second_arm_line()) {
        return _second_arm_.line.get_last();
      }
      return _second_arm_.helix.get_last();
    }

    geomtools::vector_3d two_arms_curve_3d::get_kink_point() const
    {
      DT_THROW_IF(_first_arm_.code == SHAPE_CODE_INVALID, std::logic_error, "Invalid first arm!");
      if (is_first_arm_line()) {
        return _first_arm_.line.get_last();
      }
      return _first_arm_.helix.get_last();
    }

    const geomtools::line_3d & two_arms_curve_3d::get_first_arm_line() const
    {
      DT_THROW_IF(! is_first_arm_line(), std::logic_error, "First arm is not a line!");
      return _first_arm_.line;
    }

    const geomtools::line_3d & two_arms_curve_3d::get_second_arm_line() const
    {
      DT_THROW_IF(! is_second_arm_line(), std::logic_error, "Second arm is not a line!");
      return _second_arm_.line;
    }
      
    const geomtools::helix_3d & two_arms_curve_3d::get_first_arm_helix() const
    {
      DT_THROW_IF(! is_first_arm_helix(), std::logic_error, "First arm is not a helix!");
      return _first_arm_.helix;
    }
 
    const geomtools::helix_3d & two_arms_curve_3d::get_second_arm_helix() const
    {
      DT_THROW_IF(! is_second_arm_helix(), std::logic_error, "Second arm is not a helix!");
      return _second_arm_.helix;
    }

    bool two_arms_curve_3d::is_on_curve(const geomtools::vector_3d & position_,
                                        double tolerance_) const
    {
      DT_THROW_IF(! is_valid(), std::logic_error, "Invalid 2-arms curve!");
      if (is_first_arm_line() and _first_arm_.line.is_on_curve(position_, tolerance_)) return true;
      if (is_first_arm_helix() and _first_arm_.helix.is_on_curve(position_, tolerance_)) return true;
      if (is_second_arm_line() and _second_arm_.line.is_on_curve(position_, tolerance_)) return true;
      if (is_second_arm_helix() and _second_arm_.helix.is_on_curve(position_, tolerance_)) return true;
      return false;
    }
   
    geomtools::vector_3d two_arms_curve_3d::get_direction_on_curve(const geomtools::vector_3d & position_) const
    {
      DT_THROW_IF(! is_valid(), std::logic_error, "Invalid 2-arms curve!");
      geomtools::vector_3d dir;
      geomtools::invalidate(dir);
      if (is_first_arm_line()) {
        if (_first_arm_.line.is_on_curve(position_)) {
          dir = _first_arm_.line.get_direction_on_curve(position_);
          if (geomtools::is_valid(dir)) return dir;
        }
      }
      if (is_second_arm_line()) {
        if (_second_arm_.line.is_on_curve(position_)) {
          dir = _second_arm_.line.get_direction_on_curve(position_);
          if (geomtools::is_valid(dir)) return dir;
        }
      }
      if (is_first_arm_helix()) {
        if (_first_arm_.helix.is_on_curve(position_)) {
          dir = _first_arm_.helix.get_direction_on_curve(position_);
          if (geomtools::is_valid(dir)) return dir;
        }
      }
      if (is_second_arm_helix()) {
        if (_second_arm_.helix.is_on_curve(position_)) {
          dir = _second_arm_.helix.get_direction_on_curve(position_);
          if (geomtools::is_valid(dir)) return dir;
        }
      }
      return dir;
    }
 
    std::string two_arms_curve_3d::get_shape_name() const
    {
      return two_arms_curve_3d::two_arms_label();
    }
   
    void two_arms_curve_3d::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) {
        indent = indent_;
      }
      i_object_3d::tree_dump(out_, title_, indent_, true);

      out_ << indent << datatools::i_tree_dumpable::tag
           << "First arm : \""
           << to_string(_first_arm_.code) << '"'
           << std::endl;

      if (is_first_arm_line()) {
        _first_arm_.line.tree_dump(out_, "", indent_ + tags::skip_item());
      }

      if (is_first_arm_helix()) {
        _first_arm_.helix.tree_dump(out_, "", indent_ + tags::skip_item());
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Second arm : \""
           << to_string(_second_arm_.code) << '"'
           << std::endl;

      if (is_second_arm_line()) {
        _second_arm_.line.tree_dump(out_, "", indent_ + tags::skip_item());
      }
 
      if (is_second_arm_helix()) {
        _second_arm_.helix.tree_dump(out_, "", indent_ + tags::skip_item());
      }

      if (is_valid()) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "First point : " << geomtools::to_xyz(get_first_point())
             << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Kink point : " << geomtools::to_xyz(get_kink_point())
             << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Last point : " << geomtools::to_xyz(get_last_point())
             << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Length : " << get_length() / CLHEP::mm << " mm"
             << std::endl;
      }
    
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Valid : " << std::boolalpha << is_valid()
           << std::endl;
      
      return;
    }

    double two_arms_curve_3d::get_length(uint32_t flags_) const
    {
      DT_THROW_IF(! is_valid(), std::logic_error, "Invalid 2-arms curve!");
      double length = 0.0;
      if (flags_ & PATH_FIRST_ARM) {
        if (is_first_arm_line()) {
          length += _first_arm_.line.get_length();
        }
        if (is_first_arm_helix()) {
          length += _first_arm_.helix.get_length();
        }
      }
      if (flags_ & PATH_SECOND_ARM) {
        if (is_second_arm_line()) {
          length += _second_arm_.line.get_length();
        }
        if (is_second_arm_helix()) {
          length += _second_arm_.helix.get_length();
        }
      }
      return length;
    }
  
    void two_arms_curve_3d::generate_wires_self(geomtools::wires_type & wires_,
                                                uint32_t options_) const
    {
      DT_THROW_IF(! is_valid(), std::logic_error, "Invalid 2-arms curve!");
      if (is_first_arm_line()) {
        _first_arm_.line.generate_wires_self(wires_, options_);    
      }
      if (is_first_arm_helix()) {
        _first_arm_.helix.generate_wires_self(wires_, options_);    
      }
      if (is_second_arm_line()) {
        _second_arm_.line.generate_wires_self(wires_, options_);    
      }
      if (is_second_arm_helix()) {
        _second_arm_.helix.generate_wires_self(wires_, options_);    
      }
      return;
    }
 
  }  // namespace geometry

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

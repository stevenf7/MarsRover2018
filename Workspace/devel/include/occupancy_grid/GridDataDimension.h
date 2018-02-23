// Generated by gencpp from file occupancy_grid/GridDataDimension.msg
// DO NOT EDIT!


#ifndef OCCUPANCY_GRID_MESSAGE_GRIDDATADIMENSION_H
#define OCCUPANCY_GRID_MESSAGE_GRIDDATADIMENSION_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace occupancy_grid
{
template <class ContainerAllocator>
struct GridDataDimension_
{
  typedef GridDataDimension_<ContainerAllocator> Type;

  GridDataDimension_()
    : label()
    , size(0)
    , stride(0)  {
    }
  GridDataDimension_(const ContainerAllocator& _alloc)
    : label(_alloc)
    , size(0)
    , stride(0)  {
  (void)_alloc;
    }



   typedef std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other >  _label_type;
  _label_type label;

   typedef uint32_t _size_type;
  _size_type size;

   typedef uint32_t _stride_type;
  _stride_type stride;




  typedef boost::shared_ptr< ::occupancy_grid::GridDataDimension_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::occupancy_grid::GridDataDimension_<ContainerAllocator> const> ConstPtr;

}; // struct GridDataDimension_

typedef ::occupancy_grid::GridDataDimension_<std::allocator<void> > GridDataDimension;

typedef boost::shared_ptr< ::occupancy_grid::GridDataDimension > GridDataDimensionPtr;
typedef boost::shared_ptr< ::occupancy_grid::GridDataDimension const> GridDataDimensionConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::occupancy_grid::GridDataDimension_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >::stream(s, "", v);
return s;
}

} // namespace occupancy_grid

namespace ros
{
namespace message_traits
{



// BOOLTRAITS {'IsFixedSize': False, 'IsMessage': True, 'HasHeader': False}
// {'std_msgs': ['/opt/ros/kinetic/share/std_msgs/cmake/../msg'], 'occupancy_grid': ['/home/wmmc/MarsRover2018/Workspace/src/occupancy_grid/msg']}

// !!!!!!!!!!! ['__class__', '__delattr__', '__dict__', '__doc__', '__eq__', '__format__', '__getattribute__', '__hash__', '__init__', '__module__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_parsed_fields', 'constants', 'fields', 'full_name', 'has_header', 'header_present', 'names', 'package', 'parsed_fields', 'short_name', 'text', 'types']




template <class ContainerAllocator>
struct IsFixedSize< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::occupancy_grid::GridDataDimension_<ContainerAllocator> const>
  : FalseType
  { };

template <class ContainerAllocator>
struct IsMessage< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::occupancy_grid::GridDataDimension_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::occupancy_grid::GridDataDimension_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
{
  static const char* value()
  {
    return "4cd0c83a8683deae40ecdac60e53bfa8";
  }

  static const char* value(const ::occupancy_grid::GridDataDimension_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0x4cd0c83a8683deaeULL;
  static const uint64_t static_value2 = 0x40ecdac60e53bfa8ULL;
};

template<class ContainerAllocator>
struct DataType< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
{
  static const char* value()
  {
    return "occupancy_grid/GridDataDimension";
  }

  static const char* value(const ::occupancy_grid::GridDataDimension_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
{
  static const char* value()
  {
    return "string label\n\
uint32 size\n\
uint32 stride\n\
";
  }

  static const char* value(const ::occupancy_grid::GridDataDimension_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.label);
      stream.next(m.size);
      stream.next(m.stride);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct GridDataDimension_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::occupancy_grid::GridDataDimension_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::occupancy_grid::GridDataDimension_<ContainerAllocator>& v)
  {
    s << indent << "label: ";
    Printer<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::stream(s, indent + "  ", v.label);
    s << indent << "size: ";
    Printer<uint32_t>::stream(s, indent + "  ", v.size);
    s << indent << "stride: ";
    Printer<uint32_t>::stream(s, indent + "  ", v.stride);
  }
};

} // namespace message_operations
} // namespace ros

#endif // OCCUPANCY_GRID_MESSAGE_GRIDDATADIMENSION_H

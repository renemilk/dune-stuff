#ifndef DUNE_STUFF_FUNCTION_HH
#define DUNE_STUFF_FUNCTION_HH

#include <string>
#include <vector>

#include <dune/common/parametertree.hh>
#include <dune/common/exceptions.hh>

#include <dune/stuff/common/color.hh>

#include "functions/interfaces.hh"
#include "functions/checkerboard.hh"
#include "functions/expression.hh"
#include "functions/spe10.hh"
#include "functions/constant.hh"

namespace Dune {
namespace Stuff {


/**
 *  \brief      This is a way to create new functions fulfilling the FunctionInterface using their string identifier.
 *
 *  \attention  This class will not compile for all dimensions. The errors should give you a hint which specializations
 *              are needed below.
 */
template< class D, int d, class R, int rR, int rC = 1 >
class Functions
{
public:
  static std::vector< std::string > available()
  {
    return {
        FunctionCheckerboard< D, d, R, rR, rC >::static_id()
      , FunctionExpression< D, d, R, rR, rC >::static_id()
      , FunctionConstant< D, d, R, rR, rC >::static_id()
      , FunctionSpe10Model1< D, d, R, rR, rC >::static_id()
    };
  } // ... available(...)

  static Dune::ParameterTree defaultSettings(const std::string type = available()[0])
  {
    if (type == FunctionCheckerboard< D, d, R, rR, rC >::static_id())
      return FunctionCheckerboard< D, d, R, rR, rC >::defaultSettings();
    else if (type == FunctionExpression< D, d, R, rR, rC >::static_id())
      return FunctionExpression< D, d, R, rR, rC >::defaultSettings();
    else if (type == FunctionConstant< D, d, R, rR, rC >::static_id())
      return FunctionConstant< D, d, R, rR, rC >::defaultSettings();
    else if (type == FunctionSpe10Model1< D, d, R, rR, rC >::static_id())
      return FunctionSpe10Model1< D, d, R, rR, rC >::defaultSettings();
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... defaultSettings(...)

  static FunctionInterface< D, d, R, rR, rC >* create(const std::string type = available()[0],
                                                      const Dune::ParameterTree settings = defaultSettings())
  {
    if (type == FunctionCheckerboard< D, d, R, rR, rC >::static_id())
      return FunctionCheckerboard< D, d, R, rR, rC >::create(settings);
    else if (type == FunctionExpression< D, d, R, rR, rC >::static_id())
      return FunctionExpression< D, d, R, rR, rC >::create(settings);
    else if (type == FunctionConstant< D, d, R, rR, rC >::static_id())
      return FunctionConstant< D, d, R, rR, rC >::create(settings);
    else if (type == "function.spe10.model1")
      return FunctionSpe10Model1< D, d, R, rR, rC >::create(settings);
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... create(...)
}; // class Functions


template< class D, class R >
class Functions< D, 1, R, 1, 1 >
{
public:
  static std::vector< std::string > available()
  {
    return {
        FunctionCheckerboard< D, 1, R, 1, 1 >::static_id()
      , FunctionExpression< D, 1, R, 1, 1 >::static_id()
      , FunctionConstant< D, 1, R, 1, 1 >::static_id()
    };
  } // ... available(...)

  static Dune::ParameterTree defaultSettings(const std::string type = available()[0])
  {
    if (type == FunctionCheckerboard< D, 1, R, 1, 1 >::static_id())
      return FunctionCheckerboard< D, 1, R, 1, 1 >::defaultSettings();
    else if (type == FunctionExpression< D, 1, R, 1, 1 >::static_id())
      return FunctionExpression< D, 1, R, 1, 1 >::defaultSettings();
    else if (type == FunctionConstant< D, 1, R, 1, 1 >::static_id())
      return FunctionConstant< D, 1, R, 1, 1 >::defaultSettings();
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... defaultSettings(...)

  static FunctionInterface< D, 1, R, 1, 1 >* create(const std::string type = available()[0],
                                                      const Dune::ParameterTree settings = defaultSettings())
  {
    if (type == FunctionCheckerboard< D, 1, R, 1, 1 >::static_id())
      return FunctionCheckerboard< D, 1, R, 1, 1 >::create(settings);
    else if (type == FunctionExpression< D, 1, R, 1, 1 >::static_id())
      return FunctionExpression< D, 1, R, 1, 1 >::create(settings);
    else if (type == FunctionConstant< D, 1, R, 1, 1 >::static_id())
      return FunctionConstant< D, 1, R, 1, 1 >::create(settings);
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... create(...)
}; // class Functions< D, 1, R, 1, 1 >


template< class D, class R >
class Functions< D, 3, R, 1, 1 >
{
public:
  static std::vector< std::string > available()
  {
    return {
        FunctionCheckerboard< D, 3, R, 1, 1 >::static_id()
      , FunctionExpression< D, 3, R, 1, 1 >::static_id()
      , FunctionConstant< D, 3, R, 1, 1 >::static_id()
    };
  } // ... available(...)

  static Dune::ParameterTree defaultSettings(const std::string type = available()[0])
  {
    if (type == FunctionCheckerboard< D, 3, R, 1, 1 >::static_id())
      return FunctionCheckerboard< D, 3, R, 1, 1 >::defaultSettings();
    else if (type == FunctionExpression< D, 3, R, 1, 1 >::static_id())
      return FunctionExpression< D, 3, R, 1, 1 >::defaultSettings();
    else if (type == FunctionConstant< D, 3, R, 1, 1 >::static_id())
      return FunctionConstant< D, 3, R, 1, 1 >::defaultSettings();
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... defaultSettings(...)

  static FunctionInterface< D, 3, R, 1, 1 >* create(const std::string type = available()[0],
                                                      const Dune::ParameterTree settings = defaultSettings())
  {
    if (type == FunctionCheckerboard< D, 3, R, 1, 1 >::static_id())
      return FunctionCheckerboard< D, 3, R, 1, 1 >::create(settings);
    else if (type == FunctionExpression< D, 3, R, 1, 1 >::static_id())
      return FunctionExpression< D, 3, R, 1, 1 >::create(settings);
    else if (type == FunctionConstant< D, 3, R, 1, 1 >::static_id())
      return FunctionConstant< D, 3, R, 1, 1 >::create(settings);
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... create(...)
}; // class Functions< D, 3, R, 1, 1 >


template< class D, int d, class R, int r >
class Functions< D, d, R, r, 1 >
{
public:
  static std::vector< std::string > available()
  {
    return {
        FunctionExpression< D, d, R, r, 1 >::static_id()
    };
  } // ... available(...)

  static Dune::ParameterTree defaultSettings(const std::string type = available()[0])
  {
    if (type == FunctionExpression< D, d, R, r, 1 >::static_id())
      return FunctionExpression< D, d, R, r, 1 >::defaultSettings();
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... defaultSettings(...)

  static FunctionInterface< D, d, R, r, 1 >* create(const std::string type = available()[0],
                                                      const Dune::ParameterTree settings = defaultSettings())
  {
    if (type == FunctionExpression< D, d, R, r, 1 >::static_id())
      return FunctionExpression< D, d, R, r, 1 >::create(settings);
    else
      DUNE_THROW(Dune::RangeError,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " unknown function '" << type << "' requested!");
  } // ... create(...)
}; // class Functions< D, d, R, r, 1 >


} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FUNCTION_HH

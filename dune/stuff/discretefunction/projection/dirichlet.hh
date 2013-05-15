#ifndef DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH
#define DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

#include <vector>

#include <dune/stuff/grid/boundaryinfo.hh>
#include <dune/stuff/grid/intersection.hh>
#include <dune/stuff/function/interface.hh>
#include <dune/stuff/common/color.hh>

#if HAVE_DUNE_DETAILED_DISCRETIZATIONS
#include <dune/detailed/discretizations/space/continuouslagrange/fem.hh>
#include <dune/detailed/discretizations/discretefunction/default.hh>
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS

namespace Dune {
namespace Stuff {
namespace DiscreteFunction {


#if HAVE_DUNE_DETAILED_DISCRETIZATIONS
template< class GridPartType, int polOrder, class RangeFieldType, class VectorImp >
void project(const Dune::Stuff::GridboundaryInterface< typename GridPartType::GridViewType >& boundaryInfo,
             const Dune::Stuff::GenericStationaryFunctionInterface< typename GridPartType::ctype, GridPartType::dimension, RangeFieldType, 1, 1 >& source,
             Dune::Detailed::Discretizations::DiscreteFunctionDefault<
                Dune::Detailed::Discretizations::ContinuousLagrangeSpace::FemWrapper< GridPartType, polOrder, RangeFieldType, 1, 1 >,
                VectorImp
             >& target)
{
  typedef Dune::Detailed::Discretizations::ContinuousLagrangeSpace::FemWrapper< GridPartType, polOrder, RangeFieldType, 1, 1 > SpaceType;
  // checks
  if (source.parametric())
    DUNE_THROW(Dune::NotImplemented,
               "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
               << " not implemented for parametric functions!");
  // clear target function
  target.vector()->backend() *= RangeFieldType(0);
  // walk the grid
  const GridPartType& gridPart = target.space().gridPart();
  const auto entityEndIt = gridPart.template end< 0 >();
  for (auto entityIt = gridPart.template begin< 0 >(); entityIt != entityEndIt; ++entityIt) {
    const auto& entity = *entityIt;
    const auto& geometry = entity.geometry();
    // only consider entities with boundary intersections
    if(entity.hasBoundaryIntersections()) {
      const auto sourceLocalFunction = source.localFunction(entity);
      auto targetLocalFunction = target.localFunction(entity);
      auto targetLocalDofVector = targetLocalFunction.vector();
      const auto lagrangePointSet = target.space().backend().lagrangePointSet(entity);
      // get the lagrange points' coordinates
      typedef typename SpaceType::BackendType::LagrangePointSetType::CoordinateType LagrangePointCoordinateType;
      std::vector< LagrangePointCoordinateType > lagrangePoints(lagrangePointSet.nop(),
                                                                LagrangePointCoordinateType(0));
      for (size_t ii = 0; ii < lagrangePointSet.nop(); ++ii)
        lagrangePoints[ii] = geometry.global(lagrangePointSet.point(ii));
      // walk the intersections
      const auto intersectionEndIt = gridPart.iend(entity);
      for (auto intersectionIt = gridPart.ibegin(entity); intersectionIt != intersectionEndIt; ++intersectionIt) {
        const auto& intersection = *intersectionIt;
        // only consider dirichlet boundary intersection
        if (boundaryInfo.dirichlet(intersection)) {
          // loop over all lagrange points
          for (size_t ii = 0; ii < lagrangePointSet.nop(); ++ii) {
            // if dof lies on the boundary intersection
            if (Dune::Stuff::Grid::intersectionContains(intersection, lagrangePoints[ii])) {
              // set the corresponding target dof
              targetLocalDofVector.set(ii, sourceLocalFunction.evaluate(lagrangePointSet.point(ii)));
            } // if dof lies on the boundary intersection
          } // loop over all lagrange points
        } // only consider dirichlet boundary intersection
      } // walk the intersections
    } // only consider entities with boundary intersection
  } // walk the grid
} // static void project(...)
#endif // HAVE_DUNE_DETAILED_DISCRETIZATIONS


} // namespace DiscreteFunction
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_DISCRETEFUNCTION_PROJECTION_DIRICHLET_HH

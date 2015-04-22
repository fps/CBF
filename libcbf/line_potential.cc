/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

/*
 * line_potential.cc
 *
 * It controls the position of the end-effector to be on a line that is consisted
 * with a "reference" point and line direction
 *
 *  Created on Mar. 12, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#include "cbf/line_potential.h"

#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

#include <Eigen/Dense>

namespace CBF {

LinePotential::LinePotential()
{
  m_LineDirection = FloatVector(3);
  m_LineDirection(1) = 1.0;
}

Float LinePotential::norm(const FloatVector &v)
{
  return v.norm();
}

Float LinePotential::distance(const FloatVector &v1, const FloatVector &v2)
{
  Eigen::Vector3d x0,x1,x2;

  for(int i=0; i<3; i++)
  {
    x0(i) = v1(i);
    x1(i) = v2(i);
    x2(i) = v2(i)+m_LineDirection(i);
  }

  return ((x0-x1).cross(x0-x2)).norm()/(x2-x1).norm();
}

void LinePotential::gradient (
  FloatVector &result,
  const std::vector<FloatVector > &references,
  const FloatVector &input) {

  Float min_dist = std::numeric_limits<Float>::max();
  unsigned int min_index = 0;

  for (unsigned int i = 0; i < references.size(); ++i) {
    Float dist = distance(input, references[i]);
    if (dist < min_dist) {
      min_index = i;
      min_dist = dist;
    }
  }

  // find a normal vector from the input vector to the line
  FloatVector lProjectionVec(3);
  FloatVector lReference(3);

  lProjectionVec = input- references[min_index];
  lProjectionVec = m_LineDirection* m_LineDirection.dot(lProjectionVec);

  lReference = references[min_index]+lProjectionVec;

  result  = (lReference - input)*2.0;
}

void LinePotential::integration (
    FloatVector &nextpos,
    const FloatVector &currentpos,
    const FloatVector &currentvel,
    const Float timestep)
{
  nextpos = currentpos + 0.5*currentvel*timestep;
}


void LinePotential::setLineDirection(const FloatVector &v)
{
  m_LineDirection = v;
  m_LineDirection.normalize();
}

void LinePotential::setInputVelocity(const FloatVector &Velocity)
{

}

} // namespace
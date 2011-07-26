// BEGIN LICENSE BLOCK
/*
Copyright (c) 2009, UT-Battelle, LLC
All rights reserved

[DMRG++, Version 2.0.0]
[by G.A., Oak Ridge National Laboratory]

UT Battelle Open Source Software License 11242008

OPEN SOURCE LICENSE

Subject to the conditions of this License, each
contributor to this software hereby grants, free of
charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), a
perpetual, worldwide, non-exclusive, no-charge,
royalty-free, irrevocable copyright license to use, copy,
modify, merge, publish, distribute, and/or sublicense
copies of the Software.

1. Redistributions of Software must retain the above
copyright and license notices, this list of conditions,
and the following disclaimer.  Changes or modifications
to, or derivative works of, the Software should be noted
with comments and the contributor and organization's
name.

2. Neither the names of UT-Battelle, LLC or the
Department of Energy nor the names of the Software
contributors may be used to endorse or promote products
derived from this software without specific prior written
permission of UT-Battelle.

3. The software and the end-user documentation included
with the redistribution, with or without modification,
must include the following acknowledgment:

"This product includes software produced by UT-Battelle,
LLC under Contract No. DE-AC05-00OR22725  with the
Department of Energy."
 
*********************************************************
DISCLAIMER

THE SOFTWARE IS SUPPLIED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER, CONTRIBUTORS, UNITED STATES GOVERNMENT,
OR THE UNITED STATES DEPARTMENT OF ENERGY BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

NEITHER THE UNITED STATES GOVERNMENT, NOR THE UNITED
STATES DEPARTMENT OF ENERGY, NOR THE COPYRIGHT OWNER, NOR
ANY OF THEIR EMPLOYEES, REPRESENTS THAT THE USE OF ANY
INFORMATION, DATA, APPARATUS, PRODUCT, OR PROCESS
DISCLOSED WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS.

*********************************************************


*/
// END LICENSE BLOCK
/** \ingroup DMRG */
/*@{*/

/*! \file TargetStructureParams.h
 *
 *  This is a structure to represent the parameters of the TimeStep Evolution
 *  algorithm. Don't add functions to this class because
 *  this class's data is all public
 */
#ifndef TIMESTEP_PARAMS_H
#define TIMESTEP_PARAMS_H

#include "TargetParamsCommon.h"

namespace Dmrg {
	//! Coordinates reading of TargetSTructure from input file
	template<typename ModelType>
	class TimeStepParams : public TargetParamsCommon<ModelType> {
		public:
			typedef TargetParamsCommon<ModelType> TargetParamsCommonType;
			typedef typename ModelType::RealType RealType;

			static size_t const PRODUCT = TargetParamsCommonType::PRODUCT;

			template<typename IoInputter>
			TimeStepParams(IoInputter& io,const ModelType& model)
				: TargetParamsCommonType(io,model),tau(0),timeSteps(0),
				  advanceEach(0)
			{
				//io.rewind();
				this->concatenation = PRODUCT;
				tau <= io["programSpecific"]["DMRGPP"]["Dynamic"]["TSPTau"]; //LINE ADDED FOR JSON FORMAT NEEDS WORK 
				timeSteps <= io["programSpecific"]["DMRGPP"]["Dynamic"]["TSPTimeSteps"]; //LINE ADDED FOR JSON FORMAT NEEDS WORK
				advanceEach <= io["programSpecific"]["DMRGPP"]["Dynamic"]["TSPAdvanceEach"]; //LINE ADDED FOR JSON FORMAT NEEDS WORK
			}
			
			RealType tau;
			size_t timeSteps;
			size_t advanceEach;
	}; // class TimeStepParams
	
	template<typename ModelType>
	inline std::ostream&
	operator<<(std::ostream& os,const TimeStepParams<ModelType>& t)
	{
		os<<"#TargetParams.type=TimeStep\n";
		os<<"#TargetParams.tau="<<t.tau<<"\n";
		os<<"#TargetParams.timeSteps="<<t.timeSteps<<"\n";
		os<<"#TargetParams.advanceEach="<<t.advanceEach<<"\n";
		const typename TimeStepParams<ModelType>::TargetParamsCommonType& tp = t;
		os<<tp;
		return os;
	}
} // namespace Dmrg 

/*@}*/
#endif // TIMESTEP_PARAMS_H

// BEGIN LICENSE BLOCK
/*
 * Copyright (c) 2009, UT-Battelle, LLC
 * All rights reserved
 * 
 * [DMRG++, Version 2.0.0]
 * [by G.A., Oak Ridge National Laboratory]
 * 
 * UT Battelle Open Source Software License 11242008
 * 
 * OPEN SOURCE LICENSE
 * 
 * Subject to the conditions of this License, each
 * contributor to this software hereby grants, free of
 * charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), a
 * perpetual, worldwide, non-exclusive, no-charge,
 * royalty-free, irrevocable copyright license to use, copy,
 * modify, merge, publish, distribute, and/or sublicense
 * copies of the Software.
 * 
 * 1. Redistributions of Software must retain the above
 * copyright and license notices, this list of conditions,
 * and the following disclaimer.  Changes or modifications
 * to, or derivative works of, the Software should be noted
 * with comments and the contributor and organization's
 * name.
 * 
 * 2. Neither the names of UT-Battelle, LLC or the
 * Department of Energy nor the names of the Software
 * contributors may be used to endorse or promote products
 * derived from this software without specific prior written
 * permission of UT-Battelle.
 * 
 * 3. The software and the end-user documentation included
 * with the redistribution, with or without modification,
 * must include the following acknowledgment:
 * 
 * "This product includes software produced by UT-Battelle,
 * LLC under Contract No. DE-AC05-00OR22725  with the
 * Department of Energy."
 * 
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

/*! \file IoBridge.h
 * 
 *  A bridge in the input/output to be able to read old format plain 
 * input files
 * 
 */
#ifndef IO_BRIDGE_H
#define IO_BRIDGE_H

#include "IoSimple.h"
#include "Matrix.h"
#include "JsonParser.h"
#include "JsonReader.h"
#include "FiniteLoop.h"
#include <string>

namespace Dmrg {

	class	IoBridge {
		enum {PLAIN,JSON};
		typedef IoBridge ThisType;

	public:
		typedef PsimagLite::IoSimple::In OldIoType;
		typedef dca::JsonReader JsonIoType;
		
		IoBridge(const std::string& filename)
		: type_(JSON),equalSign_("=")
		{
			if (filename.find(".inp")!=std::string::npos) type_=PLAIN;
			
			if (type_==PLAIN) {
				oldIo_ = new OldIoType(filename);
				return;
			}
			jsonIo_ = new JsonIoType(filename);
		}
		
		const ThisType& searchFor(const std::string& str)
		{
			savedString_ = str;
			return *this;
		}

		void load(std::pair<size_t,size_t>& x) const
		{
			std::vector<size_t> v(2);

			oldIo_->readKnownSize(v,savedString_);
			x.first = v[0];
			x.second = v[1];
		}
		
		void load(int& x) const
		{
			oldIo_->readline(x,savedString_+equalSign_);
		}
		
		void load(size_t& x) const
		{
			oldIo_->readline(x,savedString_+equalSign_);
		}
		
		void load(double& x) const
		{
			oldIo_->readline(x,savedString_+equalSign_);
		}
		
		template<typename SomeT>
		void load(PsimagLite::Matrix<SomeT>& x) const
		{
			oldIo_->readMatrix(x,savedString_);
		}
		
		void load(std::string& x) const
		{
			oldIo_->readline(x,savedString_+equalSign_);
		}

		template<typename SomeT>
		void load(std::vector<SomeT>& x) const
		{
			oldIo_->read(x,savedString_);
		}
	
		void rewind()
		{
			if (type_==PLAIN) oldIo_->rewind();
		}	
		
		template<typename SomeType2>
		friend SomeType2& operator<=(SomeType2& lhs,const IoBridge& rhs);
		
		friend std::vector<FiniteLoop>& operator<=(std::vector<FiniteLoop>& lhs,const IoBridge& rhs);

		template<typename SomeType2>
		friend std::pair<SomeType2,SomeType2>& operator<=(std::pair<SomeType2,SomeType2>& lhs,const IoBridge& rhs);
		
		template<typename SomeType2>
		friend PsimagLite::Matrix<SomeType2>& operator<=(PsimagLite::Matrix<SomeType2>& lhs,const IoBridge& rhs);

	private:
		size_t type_;
		std::string equalSign_;
		std::string savedString_;
		OldIoType* oldIo_;
		JsonIoType* jsonIo_;
	}; // class IoBridge

	template<typename SomeType>
	PsimagLite::Matrix<SomeType>& operator<=(PsimagLite::Matrix<SomeType>& lhs,const IoBridge& rhs)
	{
		if (rhs.type_==IoBridge::PLAIN) {
			rhs.load(lhs);
			return lhs;
		}
		dca::operator<=(lhs, rhs.jsonIo_->searchFor(rhs.savedString_));
		return lhs;
	}
	
	template<typename SomeType>
	SomeType& operator<=(SomeType& lhs,const IoBridge& rhs)
	{
		if (rhs.type_==IoBridge::PLAIN) {
			rhs.load(lhs);
			return lhs;
		}
		operator<=(lhs, rhs.jsonIo_->searchFor(rhs.savedString_));
		return lhs;
	}
	
	std::vector<FiniteLoop>& operator<=(std::vector<FiniteLoop>& lhs,const IoBridge& rhs)
	{
		if (rhs.type_==IoBridge::PLAIN) {
			rhs.load(lhs);
			return lhs;
		}
		if (rhs.savedString_ != "FiniteLoops") 
			throw std::runtime_error("operator<= something wrong while reading finite loops with json reader\n");
			
		std::vector<int> tmpVec;
		tmpVec <= rhs.jsonIo_->searchFor("FiniteLoops");
		size_t i = 0;
		size_t j = 0;
		if (tmpVec.size() == 0 && tmpVec.size()%3 !=0) throw std::runtime_error("Error reading Finite Loops\n");
		size_t numberOfFiniteLoops = tmpVec.size()/3;
		lhs.resize(numberOfFiniteLoops);
			
		while (i<tmpVec.size()) {
			lhs[j].stepLength = tmpVec[i++];
			lhs[j].keptStates = tmpVec[i++];
			lhs[j].saveOption = tmpVec[i++];
			j++;
		}
		return lhs;
	}

	template<typename T>
	std::pair<T,T>& operator<=(std::pair<T,T>& lhs,const IoBridge& rhs)
	{
		if (rhs.type_==IoBridge::PLAIN) {
			rhs.load(lhs);
			return lhs;
		}
		std::vector<T> v;
		v <= rhs.jsonIo_->searchFor(rhs.savedString_);
		if (v.size()!=2) throw std::runtime_error("IoBridge: failed reading std::pair\n");
		lhs.first = v[0];
		lhs.second = v[1];
		return lhs;
	}
	
} // namespace Dmrg

/*@}*/
#endif // IO_BRIDGE_H


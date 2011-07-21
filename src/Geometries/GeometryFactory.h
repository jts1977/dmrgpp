// BEGIN LICENSE BLOCK
/*
Copyright (c) 2009, UT-Battelle, LLC
All rights reserved

[DMRG++, Version 2.4.0]
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

/*! \file GeometryBase.h
 *
 *  Well, I need to read that chapter in
 *  Alexandrescu's "Modern C++ design" again to have
 *  a decent factory here, but this will have to do for now
 *
 */
#ifndef GEOMETRY_FACTORY_H
#define GEOMETRY_FACTORY_H

#include "Chain.h"
#include "Ladder.h"
#include "LadderX.h"
#include "LadderBath.h"

namespace Dmrg {
	
	class GeometryFactory {
			static size_t refCounter_;
		public:
			enum {CHAIN,LADDER,LADDERX,LADDERBATH};
			GeometryFactory() : dirs_(0), // move to object.dirs()
						n_(0),
						chain_(0),
						ladder_(0),
						ladderx_(0),
						ladderbath_(0)
			{
			}

			GeometryFactory(const GeometryFactory& g)
			{
				dirs_=g.dirs_; // move to object.dirs()
				n_=g.n_;
				chain_=g.chain_;
				ladder_=g.ladder_;
				ladderx_=g.ladderx_;
				ladderbath_=g.ladderbath_;
				refCounter_++;
			}

			GeometryFactory& operator=(const GeometryFactory& g)
			{
				dirs_=g.dirs_; // move to object.dirs()
				n_=g.n_;
				chain_=g.chain_;
				ladder_=g.ladder_;
				ladderx_=g.ladderx_;
				ladderbath_=g.ladderbath_;
				refCounter_++;
				return *this;
			}

			~GeometryFactory()
			{
				if (refCounter_>0) {
					refCounter_--;
					return;
				}
				switch(n_) {
				case CHAIN:
					if (chain_) delete chain_;
					break;
				case LADDER:
					if (ladder_) delete ladder_;
					break;
				case LADDERX:
					if (ladderx_) delete ladderx_;
					break;
				case LADDERBATH:
					if (ladderbath_) delete ladderbath_;
					break;
				}
			}

			template<typename IoType>
			void init(IoType& io,const std::string& s,size_t linSize)
			{
				n_=getGeometry(s);
				int x=0,tmp=0;
				switch (n_) {
				case CHAIN:
					dirs_ = 1;
					chain_ = new Chain(linSize);
					break;
				case LADDER:
					dirs_ = 2;
					x<=io["programSpecific"]["DMRGPP"]["Geometry"]["LadderLeg"];//ADDED LINE FOR JSON FOMAT
					//io.readline(x,"LadderLeg=");Replaced line 
					if (x!=2) throw std::runtime_error("LadderLeg!=2 is not implememnted yet (sorry)\n");
					ladder_ = new Ladder(linSize,x);
					break;
				case LADDERX:
					dirs_ = 4;
					x<=io["programSpecific"]["DMRGPP"]["Geometry"]["LadderLeg"];//ADDED LINE FOR JSON FOMAT
					//io.readline(x,"LadderLeg=");
					if (x!=2) throw std::runtime_error("LadderLeg!=2 is not implememnted yet (sorry)\n");
					ladderx_ = new LadderX(linSize,x);
					break;
				case LADDERBATH:
					dirs_ = 3; // X,Y, and BATH
					x<=io["programSpecific"]["DMRGPP"]["Geometry"]["LadderLeg"];//ADDED LINE FOR JSON FOMAT
					//io.readline(x,"LadderLeg=");
					if (x!=2) throw std::runtime_error("LadderLeg!=2 is not implememnted yet (sorry)\n");
					tmp<=io["programSpecific"]["DMRGPP"]["Geometry"]["BathSitesPerSite"];//ADDED LINE FOR JSON FOMAT
					//io.readline(tmp,"BathSitesPerSite=");
					if (tmp<0) throw std::runtime_error("BathSitesPerSite<0 is an error\n");
					ladderbath_ = new LadderBath(linSize,x,tmp);
					break;
				default:
					throw std::runtime_error("Unknown geometry\n");
				}

			}

			size_t dirs() const { return dirs_; } // <-- move elsewhere FIXME

			size_t handle(size_t i,size_t j) const
			{
				switch (n_) {
				case CHAIN:
					return chain_->handle(i,j);
				case LADDER:
					return ladder_->handle(i,j);
				case LADDERX:
					return ladderx_->handle(i,j);
				case LADDERBATH:
					return ladderbath_->handle(i,j);
				}
				throw std::runtime_error("Unknown geometry\n");
			}


			size_t getVectorSize(size_t dirId) const
			{
				switch (n_) {
				case CHAIN:
					return chain_->getVectorSize(dirId);
				case LADDER:
					return ladder_->getVectorSize(dirId);
				case LADDERX:
					return ladderx_->getVectorSize(dirId);
				case LADDERBATH:
					return ladderbath_->getVectorSize(dirId);
				}
				throw std::runtime_error("Unknown geometry\n");
			}

			bool connected(size_t i1,size_t i2) const
			{
				switch(n_) {
				case CHAIN:
					return chain_->connected(i1,i2);
				case LADDER:
					return ladder_->connected(i1,i2);
				case LADDERX:
					return ladderx_->connected(i1,i2);
				case LADDERBATH:
					return ladderbath_->connected(i1,i2);
				}
				throw std::runtime_error("Unknown geometry\n");
			}

			size_t calcDir(size_t i1,size_t i2) const
			{
				switch (n_) {
				case CHAIN:
					return chain_->calcDir(i1,i2);
				case LADDER:
					return ladder_->calcDir(i1,i2);
				case LADDERX:
					return ladderx_->calcDir(i1,i2);
				case LADDERBATH:
					return ladderbath_->calcDir(i1,i2);
				}
				throw std::runtime_error("Unknown geometry\n");
			}

			bool fringe(size_t i,size_t smax,size_t emin) const
			{
				switch(n_) {
				case CHAIN:
					return chain_->fringe(i,smax,emin);
				case LADDER:
					return ladder_->fringe(i,smax,emin);
				case LADDERX:
					return ladderx_->fringe(i,smax,emin);
				case LADDERBATH:
					return ladderbath_->fringe(i,smax,emin);
				}
				throw std::runtime_error("Unknown geometry\n");
			}

			size_t getSubstituteSite(size_t smax,size_t emin,size_t siteNew2) const
			{
				switch(n_) {
				case CHAIN:
					return chain_->getSubstituteSite(smax,emin,siteNew2);
				case LADDER:
					return ladder_->getSubstituteSite(smax,emin,siteNew2);
				case LADDERX:
					return ladderx_->getSubstituteSite(smax,emin,siteNew2);
				case LADDERBATH:
					return ladderbath_->getSubstituteSite(smax,emin,siteNew2);
				}
				throw std::runtime_error("Unknown geometry\n");
			}

			std::string label() const
			{
				switch(n_) {
				case CHAIN:
					return chain_->label();
				case LADDER:
					return ladder_->label();
				case LADDERX:
					return ladderx_->label();
				case LADDERBATH:
					return ladderbath_->label();
				}
				throw std::runtime_error("Unknown geometry\n");
			}

		private:

			size_t getGeometry(const std::string& s) const
			{
				size_t x = 0;
				if (s=="chain") x=CHAIN;
				else if (s=="ladder") x=LADDER;
				else if (s=="ladderx") x=LADDERX;
				else if (s=="bathedcluster") x=LADDERBATH;
				else throw std::runtime_error("unknown geometry\n");
				return x;
			}

			// ATTENTION: THIS CLASS HAS CUSTOM ASSIGNMENT OPERATOR
			// AND COPY CONTRUCTORS
			size_t dirs_; // move to object.dirs()
			size_t n_;
			Chain* chain_;
			Ladder* ladder_;
			LadderX* ladderx_;
			LadderBath* ladderbath_;
	}; // class GeometryFactory

	size_t GeometryFactory::refCounter_=0;

} // namespace Dmrg 

/*@}*/
#endif // GEOMETRY_BASE_H


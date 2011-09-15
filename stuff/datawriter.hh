#ifndef TIMEAWAREDATAWRITER_HH
#define TIMEAWAREDATAWRITER_HH

#include <dune/fem/io/file/datawriter.hh>
#include <dune/grid/io/file/vtk/vtkwriter.hh>
#include <dune/fem/function/common/function.hh>
#include <dune/fem/function/common/discretefunctionadapter.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/space/dgspace.hh>
#include <dune/stuff/customprojection.hh>

namespace Stuff {

	//! a demonic analytical/discrete function hybrid that at each point evaluates to the two_norm of a given discreteFunction
	template< class DiscreteFunctionImp, int polOrder = 2 >
	class MagnitudeFunction :
			public Dune::Function< Dune::FunctionSpace< typename DiscreteFunctionImp::FunctionSpaceType::DomainFieldType,
														double,
														DiscreteFunctionImp::FunctionSpaceType::dimDomain,
														1 >,
									MagnitudeFunction< DiscreteFunctionImp, polOrder > >
	{
		typedef MagnitudeFunction< DiscreteFunctionImp, polOrder >
			ThisType;

	public:
		typedef Dune::FunctionSpace< typename DiscreteFunctionImp::FunctionSpaceType::DomainFieldType,
																double,
																DiscreteFunctionImp::FunctionSpaceType::dimDomain,
																1 >
			MagnitudeSpaceType;
		typedef Dune::Function< MagnitudeSpaceType, ThisType >
			BaseType;
		typedef DiscreteFunctionImp
			DiscreteFunctionType;

		typedef typename DiscreteFunctionType::DiscreteFunctionSpaceType::GridPartType
			GridPartType;
		typedef Dune::DiscontinuousGalerkinSpace< MagnitudeSpaceType, GridPartType, polOrder >
			MagnitudeDiscreteFunctionSpaceType;
		typedef Dune::AdaptiveDiscreteFunction< MagnitudeDiscreteFunctionSpaceType >
			MagnitudeDiscreteFunctionType;

		//! constructor taking discrete function
		MagnitudeFunction ( const DiscreteFunctionType& df )
			: BaseType(magnitude_space_),
			  discreteFunction_( df ),
			  magnitude_disretefunctionspace_( discreteFunction_.space().gridPart() ),
			  magnitude_disretefunction_( discreteFunction_.name() + "-magnitude", magnitude_disretefunctionspace_ )
		{
			Dune::BetterL2Projection
				::project( *this, magnitude_disretefunction_ );
		}

		//! virtual destructor
		virtual ~MagnitudeFunction () {}

		void evaluate (	const typename MagnitudeSpaceType::DomainType &x,
						typename MagnitudeSpaceType::RangeType &ret ) const
		{
			typename DiscreteFunctionType::FunctionSpaceType::RangeType val;
			discreteFunction_.evaluate(x,val);
			ret = val.two_norm();
		}

		const MagnitudeDiscreteFunctionType& discreteFunction() const
		{
			return magnitude_disretefunction_;
		}

	private:
		static const MagnitudeSpaceType magnitude_space_;
		const DiscreteFunctionType &discreteFunction_;
		MagnitudeDiscreteFunctionSpaceType magnitude_disretefunctionspace_;
		MagnitudeDiscreteFunctionType magnitude_disretefunction_;
	};

	template < class DF, int I > const typename MagnitudeFunction<DF,I>::MagnitudeSpaceType
		MagnitudeFunction<DF,I>::magnitude_space_ = typename MagnitudeFunction<DF,I>::MagnitudeSpaceType();
} //namespace Stuff

namespace Dune {

	/** \brief a Dune::DataWriter derivative that handles current time internaly via passed Dune::Timeprovider
		\note TimeproviderType can only be FractionalTimeProvider atm
		**/
	template < class TimeproviderType, class GridType, class OutputTupleType >
	class TimeAwareDataWriter : public DataWriter< GridType, OutputTupleType >{
		protected:
			typedef DataWriter< GridType, OutputTupleType >
				BaseType;
			using BaseType::grape;
			using BaseType::grid_;
			using BaseType::path_;
			using BaseType::datapref_;
			using BaseType::writeStep_;
			using BaseType::outputFormat_;
			using BaseType::vtk;
			using BaseType::vtkvtx;
			using BaseType::gnuplot;
			using BaseType::data_;
			using BaseType::saveTime_;
			using BaseType::myRank_;
			using BaseType::saveStep_;

	   public:
			TimeAwareDataWriter( const TimeproviderType& timeprovider,
								 const GridType& grid,
								 OutputTupleType& tuple )
				: BaseType( grid, grid.name(), tuple, timeprovider.startTime(), timeprovider.endTime() ),
				timeprovider_( timeprovider )
			{

			}

			void write() const
			{
				write( timeprovider_.time() , timeprovider_.timeStep()  );
			}

			/** \brief write given data to disc
			   \param[in] time actual time of computation
			   \param[in] timestep current number of time step
			   \param[in] data data to write (template type, should be a tuple)
			*/
			void write(double time, int /*timestep*/ ) const
			{
//				if( BaseType::willWrite( time, timestep ) )
				{
					{
						// check online display
						BaseType::display();
					}

					if( BaseType::outputFormat_ == grape )
					{
						// create new path for time step output
						std::string timeStepPath = IOInterface::createPath ( grid_.comm(),
																			 path_, datapref_ , writeStep_ );

						// for structured grids copy grid
						IOInterface::copyMacroGrid(grid_,path_,timeStepPath,datapref_);

						GrapeDataIO<GridType> dataio;
						// call output of IOTuple
						IOTuple<OutputTupleType>::output(dataio,
														 grid_ ,time, writeStep_, timeStepPath , datapref_, data_ );
					}
	#if USE_VTKWRITER
					else if ( outputFormat_ == vtk || outputFormat_ == vtkvtx )
					{
						// write data in vtk output format
						writeVTKOutput( Element<0>::get(data_) );
					}
	#endif
					else if ( outputFormat_ == gnuplot )
					{
						writeGnuPlotOutput( time );
					}
					else
					{
						DUNE_THROW(NotImplemented,"DataWriter::write: wrong output format");
					}

					// only write info for proc 0, otherwise on large number of procs
					// this is to much output
//					if(myRank_ <= 0)
//					{
//						std::cout << "DataWriter["<<myRank_<<"]::write:  time = "<< time << "  writeData: step number " << writeStep_ << std::endl;
//					}
					saveTime_ += saveStep_;
					++writeStep_;
				}
				return;
			}

		protected:
			const TimeproviderType& timeprovider_;
			static inline std::string genFilename(const std::string& path,
										   const std::string& fn,
										   int ntime,
										   int precision = 6)
			{
				std::ostringstream name;

				if(path.size() > 0)
				{
					name << path;
					name << "/";
				}
				name << fn;

				char cp[256];
				switch(precision)
				{
					case 2  : { sprintf(cp, "%02d", ntime); break; }
					case 3  : { sprintf(cp, "%03d", ntime); break; }
					case 4  : { sprintf(cp, "%04d", ntime); break; }
					case 5  : { sprintf(cp, "%05d", ntime); break; }
					case 6  : { sprintf(cp, "%06d", ntime); break; }
					case 7  : { sprintf(cp, "%07d", ntime); break; }
					case 8  : { sprintf(cp, "%08d", ntime); break; }
					case 9  : { sprintf(cp, "%09d", ntime); break; }
					case 10 : { sprintf(cp, "%010d", ntime); break; }
					default:
						{
							DUNE_THROW(Exception, "Couldn't gernerate filename with precision = "<<precision);
						}
				}
				name << cp;

				// here implicitly a string is generated
				return name.str();
			}

			template <class VTKOut>
			class VTKOutputter {
				public:
				//! Constructor
					VTKOutputter(VTKOut& vtkOut,std::string path, bool parallel,int step)
					   : vtkOut_(vtkOut),
					   path_(path),
					   parallel_(parallel),
					   step_(step)
					{}

				//! Applies the setting on every DiscreteFunction/LocalFunction pair.
					template <class DFType>
					void visit(DFType* f)
					{
						if (!f)
							return;

						//needs to in same scope as clear() ?
						Stuff::MagnitudeFunction<DFType> magnitude( *f );

						std::string name = genFilename( (parallel_) ? "" : path_, f->name(), step_ );
						if ( DFType::FunctionSpaceType::DimRange > 1 ) {
							vtkOut_.addVectorVertexData( *f );
							vtkOut_.addVectorCellData( *f );
							vtkOut_.addVertexData( magnitude.discreteFunction() );
							vtkOut_.addCellData( magnitude.discreteFunction() );
						}
						else {
							vtkOut_.addVertexData( *f );
							vtkOut_.addCellData( *f );
						}
						const bool binary_output = Parameters().getParam( "binary_vtk", true );

						if( parallel_ )
						{
							// write all data for parallel runs
							vtkOut_.pwrite( name.c_str(), path_.c_str(), "." ,
									binary_output ? Dune::VTKOptions::binaryappended
										      : Dune::VTKOptions::ascii );
						}
						else
						{
							// write all data serial
							vtkOut_.write( name.c_str(), binary_output ? Dune::VTKOptions::binaryappended
												   : Dune::VTKOptions::ascii  );
						}

						vtkOut_.clear();
					}

				private:
					VTKOut & vtkOut_;
					const std::string path_;
					const bool parallel_;
					const int step_;
			};

	#if USE_VTKWRITER
			template <class DFType>
			void writeVTKOutput(const DFType* func) const
			{

				if( !func )
					return;

				// check whether we have parallel run
				const bool parallel = (grid_.comm().size() > 1);

				// generate filename, with path only for serial run

				{
					// get grid part
					typedef typename DFType :: DiscreteFunctionSpaceType :: GridPartType GridPartType;
					const GridPartType& gridPart = func->space().gridPart();

					{
						// create vtk output handler
						typedef SubsamplingVTKIO < GridPartType > VTKIOType;
						VTKIOType vtkio ( gridPart, VTKOptions::nonconforming );

						// add all functions
						ForEachValue<OutputTupleType> forEach(data_);
						VTKOutputter< VTKIOType > io( vtkio, path_, parallel, timeprovider_.timeStep() );
						forEach.apply( io );

						// write all data

					}
				}
			}
	#endif
			struct Gnu {
				const double time_;
				const std::string path_,datapref_;
				const bool parallel_;
				const int step_;
				Gnu(const double time,
					std::string path, bool parallel,int step,std::string datapref)
										   : time_(time),
										   path_(path),
											 datapref_(datapref),
										   parallel_(parallel),
										   step_(step){}
				// write to gnuplot file format
				template <class DFType>
				void visit(const DFType* func) const
				{
					if (!func)
						return;

					typedef typename DFType :: Traits Traits;
					typedef typename Traits :: LocalFunctionType LocalFunctionType;
					typedef typename Traits :: DiscreteFunctionSpaceType DiscreteFunctionSpaceType;
					typedef typename DiscreteFunctionSpaceType :: IteratorType IteratorType;
					typedef typename DiscreteFunctionSpaceType :: GridPartType GridPartType;

					typedef typename DiscreteFunctionSpaceType :: DomainType DomainType;
					typedef typename DiscreteFunctionSpaceType :: RangeType RangeType;

					enum{ dimDomain = DiscreteFunctionSpaceType :: dimDomain };
					enum{ dimRange = DiscreteFunctionSpaceType :: dimRange };

					// generate filename
//					std::string name = genFilename( path_, datapref_, step_ );
					std::string name = genFilename( path_, datapref_, 0 );
					name += "_" + func->name() + ".gnu";
					const bool first = (time_ > 0.0);
					std::ios_base::openmode mode = first ?  std::ios_base::app : std::ios_base::out;
					std::ofstream gnuout( name.c_str(), mode );

					if ( first )
					{
						gnuout << "#";
						for (int i = 0; i < dimDomain; ++i)
							gnuout << "x_"<< i << " ";
						for (int i = 0; i < dimRange; ++i)
							gnuout << "f_"<< i << " ";
						gnuout << "time" << "\n";
					}
					// start iteration
					IteratorType endit = func->space().end();
					for (IteratorType it = func->space().begin(); it != endit; ++it) {
						CachingQuadrature<GridPartType,0> quad(*it,func->space().order());
						LocalFunctionType lf = func->localFunction(*it);
						for (size_t i=0;i<quad.nop();++i) {
							RangeType u;
							DomainType x = it->geometry().global(quad.point(i));
							lf.evaluate(quad[i],u);
							for (int i = 0; i < dimDomain; ++i)
								gnuout << x[i] << " ";
							for (int i = 0; i < dimRange; ++i)
								gnuout << u[i] << " ";
							gnuout << time_ << "\n";
						}
					}
					gnuout << "\n\n";
				}
			};

			void writeGnuPlotOutput( const double time ) const
			{
				const bool parallel = (grid_.comm().size() > 1);
				ForEachValue<OutputTupleType> forEach(data_);
				Gnu io( time, path_, parallel, timeprovider_.timeStep(),datapref_ );
				forEach.apply( io );

			}


	};

} // end namespace Dune
#endif // DATAWRITER_HH


###############################################################
######################## Google Tests #########################
###############################################################

add_executable(AsianOptionTest AsianOptionTest.cpp ${SOURCESTEST} ${HEADERSTEST})
target_link_libraries(AsianOptionTest gtest gtest_main jl_parser ${LIBS})
#
#add_executable(BasketOptionTest BasketOptionTest.cpp ${SOURCESTEST} $#{HEADERSTEST})
#target_link_libraries(BasketOptionTest gtest gtest_main jl_parser $#{LIBS})

#add_executable(PerformanceOptionTest PerformanceOptionTest.cpp $#{SOURCESTEST} ${HEADERSTEST})
#target_link_libraries(PerformanceOptionTest gtest gtest_main #jl_parser  ${LIBS})

#add_executable(MonteCarloTest MonteCarloTest.cpp ${SOURCESTEST} #{HEADERSTEST})
#target_link_libraries(MonteCarloTest gtest gtest_main jl_parser  $#{LIBS})

#add_executable(BlackScholesTest BlackScholesTest.cpp ${SOURCESTEST} $#{HEADERSTEST})
#target_link_libraries(BlackScholesTest gtest gtest_main jl_parser  #{LIBS})
#

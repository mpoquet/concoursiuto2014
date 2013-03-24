%module contest
%include "std_string.i"
%include "std_vector.i"


%{
    #include "contest.hpp"
%}


#if defined(SWIGPYTHON)
    %rename (_wrap_planets) GameData::planets();
    %rename (_wrap_scanResults) GameData::scanResults();
    %rename (_wrap_fleets) GameData::fleets();
    %rename (_wrap_enemies) GameData::enemies();
    %rename (_wrap_reports) GameData::reports();
#endif


%copyctor GameData;
%copyctor Planet;
%copyctor ScanResult;
%copyctor Fleet;
%copyctor Ennemy;
%copyctor FightReport;


%include "contest.hpp"


namespace std
{
    #if defined(SWIGPYTHON)
        %template(_wrap_PlanetList) vector<Planet>;
        %template(_wrap_ScanResultList) vector<ScanResult>;
        %template(_wrap_FleetList) vector<Fleet>;
        %template(_wrap_EnnemyList) vector<Ennemy>;
        %template(_wrap_FightReportList) vector<FightReport>;
        %template(_wrap_IntList) vector<int>;
    #else
        %template(PlanetList) vector<Planet>;
        %template(ScanResultList) vector<ScanResult>;
        %template(FleetList) vector<Fleet>;
        %template(EnnemyList) vector<Ennemy>;
        %template(FightReportList) vector<FightReport>;
        %template(IntList) vector<int>;
    #endif
}


#if defined(SWIGPYTHON)
    %extend GameData {
        %pythoncode %{
            def planets(self):
                vect = self.__wrap_planets__()
                return [i for i in vect]

            def scanResults(self):
                vect = self.__wrap_scanResults__()
                return [i for i in vect]

            def fleets(self):
                vect = self.__wrap_fleets__()
                return [i for i in vect]

            def enemies(self):
                vect = self.__wrap_enemies__()
                return [i for i in vect]

            def reports(self):
                vect = self.__wrap_reports__()
                return [i for i in vect]
        %}
    }

    %extend GameInfos {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }

    %extend Planet {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }

    %extend ScanResult {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }

    %extend Fleet {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }

    %extend Ennemy {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }

    %extend FightReport {
        %pythoncode %{
            def __repr__(self):
                return type(self).__name__+'('+', '.join([k+':'+str(v(self)) for k,v in self.__swig_getmethods__.items()])+')'
        %}
    }
#endif



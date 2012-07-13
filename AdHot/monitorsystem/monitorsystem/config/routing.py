"""Routes configuration

The more specific and detailed routes should be defined first so they
may take precedent over the more generic routes. For more information
refer to the routes manual at http://routes.groovie.org/docs/
"""
from routes import Mapper

def make_map(config):
    """Create, configure and return the routes Mapper"""
    map = Mapper(directory=config['pylons.paths']['controllers'],
                 always_scan=config['debug'])
    map.minimization = False
    map.explicit = False

    # The ErrorController route (handles 404/500 error pages); it should
    # likely stay at the top, ensuring it can always be resolved
    map.connect('/error/{action}', controller='error')
    map.connect('/error/{action}/{id}', controller='error')
  
    map.connect('/', controller='index',action='index')	 
    map.connect('/monitor/{chart}/{points}/{endtime}', controller='monitor',action='graph')
    map.connect('/monitor/{chart}/{points}', controller='monitor',action='graph')
    map.connect('/overview/{catagory}/{points}', controller='overview',action='graph')
    map.connect('/warning_report', controller='warning_report',action='graph')
    map.connect('/zoom/{dbname}/{datasource}/{resolution}/{title}/{points}/{limit}/{description}', controller='zoom_graph',action='zoomin')


    # CUSTOM ROUTES HERE

    map.connect('/{controller}/{action}')
    map.connect('/{controller}/{action}/{id}')
    map.connect('/{controller}/{action}/{chart}/{value}')
    #map.connect('/{controller}{action}/{chart}/{type}/{points}')
    map.connect('/{controller}/{action}/{dbname}/{servicename}/{value}')

    return map

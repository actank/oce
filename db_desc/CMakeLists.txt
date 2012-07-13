#project (bookshop)
ADD_OCE_TARGET (dbdesc    STATIC_LINKS arch base storm oldslice
						  SHARED_LINKS pthread mysqlpp
              DEPENDS storm oldslice test arch base
              )


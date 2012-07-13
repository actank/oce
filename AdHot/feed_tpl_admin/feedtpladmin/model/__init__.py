"""The application's model objects"""
from feedtpladmin.model.meta import Session, Base
from feedtpladmin.model.stypekeys import StypeKeys

def init_model(engine):
    """Call me before using any of the tables or classes in the model"""
    Session.configure(bind=engine)

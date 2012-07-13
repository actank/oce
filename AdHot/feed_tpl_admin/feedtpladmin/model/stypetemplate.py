from sqlalchemy import Column
from sqlalchemy.types import Integer, String

from feedtpladmin.model.meta import Base

class StypeTemplate(Base):
    __tablename__ = "tpl_content"

    id = Column(Integer, primary_key=True)
    stype = Column(Integer)
    version = Column(Integer)
    view = Column(Integer)
    template = Column(String(8192))

    def __init__(self, stype, version, view, template):
        self.stype = stype
        self.version = version
        self.view = view
        self.template = template

    def __repr__(self):
        return "<StypeTemplate('%s')" % self.template

from sqlalchemy import Column
from sqlalchemy.types import Integer, String

from feedtpladmin.model.meta import Base

class StypeKeys(Base):
    __tablename__ = "stype_keys"

    stype = Column(Integer, primary_key=True)
    version = Column(Integer, primary_key=True)
    key_list = Column(String(2048))

    def __init__(self, stype, version, key_list):
        self.stype = stype
        self.version = version
        self.key_list = key_list

    def __repr__(self):
        return "<StypeKeys('%s')" % self.title

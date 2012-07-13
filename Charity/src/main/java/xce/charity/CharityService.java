package xce.charity;

import xce.util.service.ServiceI;

public class CharityService extends ServiceI {

  @Override
  public void finishup() {

  }

  @Override
  public void initialize() {
    String path = _ic.getProperties().getPropertyWithDefault(
        "Charity.SavingPath", "charity.data");
    CharityManagerI m = new CharityManagerI(path);
    m.initialize();
    _adapter.add(m, _ic.stringToIdentity("M"));
  }
}

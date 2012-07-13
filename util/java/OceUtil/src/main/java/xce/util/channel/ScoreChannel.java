package xce.util.channel;

public class ScoreChannel extends Channel {

	protected ScoreChannel() {
		super("ScoreChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "Score");
		properties.setProperty("Ice.Default.Locator",
				"Score/Locator:default -h ScoreMaster -p 4070:"
						+ "default -h ScoreReplica1 -p 4070:"
						+ "default -h ScoreReplica2 -p 4070");
	}
}
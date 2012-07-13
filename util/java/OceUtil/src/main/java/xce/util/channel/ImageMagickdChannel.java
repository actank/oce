package xce.util.channel;

public class ImageMagickdChannel extends Channel {
	protected ImageMagickdChannel() {
		super("ImageMagickdChannel");
	}

	@Override
	protected void initLocator() {
		properties.setProperty("IceGrid.InstanceName", "ImageMagickd");
		properties.setProperty("Ice.Default.Locator",
				"ImageMagickd/Locator:default -h ImageMagickdMaster -p 4067:"
						+ "default -h ImageMagickdReplica1 -p 4067:"
						+ "default -h ImageMagickdReplica2 -p 4067");
	}
	
	@Override
	protected void initOverride() {
		super.initOverride();
		properties.setProperty("Ice.MessageSizeMax", "102400");
	}
}

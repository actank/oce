package build;

public class TimeCost {
	private final long beginTime = System.currentTimeMillis();
	private final Object name;
	private long stepTime;
	private boolean alreadyEnd = false;

	public static TimeCost begin(String name) {
		return new TimeCost(name);
	}

	private TimeCost(String name) {
		this.stepTime = this.beginTime;
		this.name = name;
	}

	public void endStep(String stepName) {
		if (alreadyEnd)
			return;
		String message = name + " - Step " + stepName + " - Cost: "
				+ (System.currentTimeMillis() - stepTime);
		stepTime = System.currentTimeMillis();
		System.out.println(message);
	}

	public void endStep(String stepName, Throwable e) {
		if (alreadyEnd) {
			return;
		}
		String message = name + " - " + stepName + " - Cost: "
				+ (System.currentTimeMillis() - stepTime);
		stepTime = System.currentTimeMillis();
		System.out.println(message);
	}

	public void endException(Throwable e) {
		if (alreadyEnd) {
			return;
		}

		System.out.println(e);
	}

	public void endFinally() {
		if (alreadyEnd) {
			return;
		}
		alreadyEnd = true;
		String message = name + " - Cost: "
				+ (System.currentTimeMillis() - beginTime);
		System.out.println(message);
	}

	@Override
	protected void finalize() throws Throwable {
		if (!alreadyEnd) {
			String message = "Finialize an TimeCost object not ended. TimeCost should alwalys be ended in finally block. "
					+ name;
			System.out.println(message);
		}
	}
}

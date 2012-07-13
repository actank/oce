package com.renren.messagepipe.producer;

import javax.jms.BytesMessage;
import javax.jms.Connection;
import javax.jms.Destination;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.Session;

import org.apache.activemq.ActiveMQConnectionFactory;

public class SyncProducer implements ExceptionListener {

	private String host;
	private int port;

	private String subject;
	private Connection connection;
	private Destination destination;
	private MessageProducer producer;
	private Session session;

	public SyncProducer(String host, int port, String subject) {
		this.host = host;
		this.port = port;
		this.subject = subject;
	}

	public boolean connect() throws JMSException {
		String url = "tcp://" + host + ":" + port;
		System.out.println(url);
		ActiveMQConnectionFactory connectionFactory = new ActiveMQConnectionFactory(
				url);
		 connectionFactory.setAlwaysSyncSend(true);
		connection = connectionFactory.createConnection();
		connection.start();
		connection.setExceptionListener(this);

		session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
		destination = session.createQueue(subject);
		producer = session.createProducer(destination);
		return true;
	}

	public boolean send(MessageData data) throws JMSException {
		BytesMessage message = session.createBytesMessage();
		message.writeBytes(data.getData());
		producer.send(message);
		return true;
	}

	public void close() {
		try {
			connection.close();
		} catch (JMSException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void onException(JMSException ex) {

		System.out.println("some exception " + ex);
		do {
			try {
				if (connect())
					break;
			} catch (Exception e) {

			}
		} while (true);
	}
}

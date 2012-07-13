package com.renren.messagepipe.consumer;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;

import javax.jms.BytesMessage;
import javax.jms.Connection;
import javax.jms.Destination;
import javax.jms.ExceptionListener;
import javax.jms.JMSException;
import javax.jms.MessageConsumer;
import javax.jms.Session;

import org.apache.activemq.ActiveMQConnection;
import org.apache.activemq.ActiveMQConnectionFactory;

import com.renren.messagepipe.common.Message;
import com.renren.messagepipe.producer.MessageData;

public class AsyncConsumer extends Thread implements ExceptionListener {

	private String host;
	private int port;

	private String subject;

	private Session session;
	private Connection connection;
	private Destination destination;
	private MessageConsumer consumer;

	public AsyncConsumer(String host, int port, String subject) {
		this.host = host;
		this.port = port;
		this.subject = subject;
	}

	public boolean connect() throws JMSException {
		ActiveMQConnectionFactory connectionFactory = new ActiveMQConnectionFactory(
				ActiveMQConnection.DEFAULT_USER,
				ActiveMQConnection.DEFAULT_PASSWORD, "tcp://" + host + ":"
						+ port);
		connectionFactory.setOptimizeAcknowledge(true);
		connectionFactory.setOptimizedMessageDispatch(true);
		connectionFactory.setAlwaysSessionAsync(true);
		Connection connection;
		connection = connectionFactory.createConnection();
		connection.setExceptionListener(this);
		connection.start();
		session = connection.createSession(false, Session.CLIENT_ACKNOWLEDGE);
		destination = session.createQueue(subject);

		consumer = session.createConsumer(destination);
		this.start();
		return true;
	}

	ArrayBlockingQueue<BytesMessage> buffers = new ArrayBlockingQueue<BytesMessage>(
			1000);

	public List<BytesMessage> receive() {
		List<BytesMessage> res = new ArrayList<BytesMessage>();
		buffers.drainTo(res);
		return res;
	}

	@Override
	public void run() {
		while (true) {
			try {
				BytesMessage message;
				if ((message = (BytesMessage) consumer.receive(1000)) != null) {
					buffers.offer(message);
				}
			} catch (JMSException e) {
				System.out.println(e);
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
					e1.printStackTrace();
				}
			}
		}

	}

	@Override
	public void onException(JMSException ex) {
		System.out.println("some exception occur : " + ex);
		do {
			try {
				if (connect())
					break;
			} catch (Exception e) {

			}
		} while (true);
	}

	public void close() {
		try {
			connection.close();
		} catch (JMSException e) {
			e.printStackTrace();
		}
	}

}

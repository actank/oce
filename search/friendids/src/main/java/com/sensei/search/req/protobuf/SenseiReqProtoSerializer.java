package com.sensei.search.req.protobuf;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

import scala.actors.threadpool.Arrays;

import com.google.common.io.ByteArrayDataOutput;
import com.google.common.primitives.Ints;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.TextFormat;
import com.kamikaze.docidset.utils.IntArray;
import com.linkedin.norbert.network.Serializer;
import com.xiaonei.xce.fof.service.impl.FriendOfFriendRequest;
import com.xiaonei.xce.fof.service.impl.FriendOfFriendResult;

public class SenseiReqProtoSerializer implements
		Serializer<FriendOfFriendRequest, FriendOfFriendResult> {
	public String requestName() {
		return SenseiRequestBPO.Request.getDescriptor().getName();
	}

	public String responseName() {
		return SenseiResultBPO.Result.getDescriptor().getName();
	}

	public byte[] requestToBytes(FriendOfFriendRequest request) {
		int pre = 0;
		try {
			ByteDataOutput out = new ByteDataOutput();
			out.writeInt(request.getUid());
			out.writeVInt(request.getFriends().size());
			for (int res : request.getFriends()) {
				out.writeVInt(res - pre);
				pre = res;
			}
			return out.toBytes();
		} catch (IOException ex) {
			ex.printStackTrace();
			return new byte[0];
		}
		// return
		// SenseiRequestBPO.Request.newBuilder().setVal(serialize(request))
		// .build().toByteArray();
	}

	public byte[] responseToBytes(FriendOfFriendResult response) {

		int pre = 0;
		try {
			ByteDataOutput out = new ByteDataOutput();
			out.writeVInt(response.getRes().length);
			for (int res : response.getRes()) {
				out.writeVInt(res - pre);
				pre = res;
			}
			return out.toBytes();
		} catch (IOException ex) {
			ex.printStackTrace();
			return new byte[0];
		}

		// int[] data = response.getRes();
		// ByteBuffer byteBuffer = ByteBuffer.allocate(data.length * 4);
		// IntBuffer intBuffer = byteBuffer.asIntBuffer();
		// intBuffer.put(data);
		//
		// byte[] array = byteBuffer.array();
		// return array;

		// return
		// SenseiResultBPO.Result.newBuilder().setVal(serialize(response))
		// .build().toByteArray();
	}

	private <T> ByteString serialize(T obj) {
		try {
			return ProtoConvertUtil.serializeOut(obj);
		} catch (TextFormat.ParseException e) {
			throw new RuntimeException(e);
		}
	}

	public FriendOfFriendRequest requestFromBytes(byte[] request) {
		FriendOfFriendRequest req = new FriendOfFriendRequest();
		if (request.length <= 0) {
			return req;
		} else {
			ByteDataInput input = new ByteDataInput(request);
			try {
				int uid = input.readInt();
				req.setUid(uid);
				int len = input.readVInt();
				List<Integer> ids = new ArrayList<Integer>(len);
				int pre = 0;
				for (int i = 0; i < len; ++i) {
					pre += input.readVInt();
					ids.add(pre);
				}
				req.setFriends(ids);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return req;
		// try {
		// return (FriendOfFriendRequest) ProtoConvertUtil
		// .serializeIn(SenseiRequestBPO.Request.newBuilder()
		// .mergeFrom(request).build().getVal());
		// } catch (TextFormat.ParseException e) {
		// throw new RuntimeException(e);
		// } catch (InvalidProtocolBufferException e) {
		// throw new RuntimeException(e);
		// }
	}

	public FriendOfFriendResult responseFromBytes(byte[] result) {
		FriendOfFriendResult res = new FriendOfFriendResult();
		if (result.length <= 0) {
			return res;
		} else {
			ByteDataInput input = new ByteDataInput(result);
			try {
				int len = input.readVInt();
				int[] ids = new int[len];
				int pre = 0;
				for (int i = 0; i < len; ++i) {
					pre += input.readVInt();
					ids[i] = pre;
				}
				res.setRes(ids);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return res;
		// int[] data = new int[result.length / 4];
		// for (int i = 0; i < data.length; i++) {
		// int sum = (result[i * 4] << 24) | (result[i * 4 + 1] << 16)
		// | (result[i * 4 + 2] << 8) | (result[i * 4 + 3]);
		// data[i] = sum;
		// }
		// FriendOfFriendResult res = new FriendOfFriendResult();
		// res.setRes(data);
		// return res;
		// try {
		// return (FriendOfFriendResult) ProtoConvertUtil
		// .serializeIn(SenseiResultBPO.Result.newBuilder()
		// .mergeFrom(result).build().getVal());
		// } catch (TextFormat.ParseException e) {
		// throw new RuntimeException(e);
		// } catch (InvalidProtocolBufferException e) {
		// throw new RuntimeException(e);
		// }
	}

	public static void main(String[] args) {
		int[] data = new int[] { 1, 2, 3, 4 };
		ByteBuffer byteBuffer = ByteBuffer.allocate(data.length * 4);
		IntBuffer intBuffer = byteBuffer.asIntBuffer();
		intBuffer.put(data);

		byte[] array = byteBuffer.array();
		System.out.println(array.length);

		int[] data2 = new int[array.length / 4];
		for (int i = 0; i < data2.length; i++) {
			int sum = (array[i * 4] << 24) | (array[i * 4 + 1] << 16)
					| (array[i * 4 + 2] << 8) | (array[i * 4 + 3]);
			System.out.println(sum);
			data2[i] = sum;
		}
	}
}

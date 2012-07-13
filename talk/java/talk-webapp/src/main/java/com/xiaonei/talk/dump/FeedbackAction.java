package com.xiaonei.talk.dump;

import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;
import org.apache.struts.action.DynaActionForm;
import org.apache.struts.upload.FormFile;

public class FeedbackAction extends Action {
	@Override
	public ActionForward execute(ActionMapping mapping, ActionForm form,
			HttpServletRequest request, HttpServletResponse response)
			throws Exception {
		// TODO Auto-generated method stub

		DynaActionForm dform = (DynaActionForm) form;
		FormFile file = (FormFile) dform.get("upload_file_minidump");

		if (file == null) {
			return null;
		}

		InputStream in = file.getInputStream();
		String filename = String.valueOf(getTime()) + ".dmp";

		OutputStream out = new FileOutputStream("/data/xntalk/xntalk.dmp/"
				+ filename);
		int n = 0;
		byte[] buffer = new byte[1024 * 16];
		while ((n = in.read(buffer, 0, 1024 * 16)) != -1) {
			out.write(buffer, 0, n);
		}
		in.close();
		out.close();
		file.destroy();
		try{
		Runtime.getRuntime().exec(
				"/data/xntalk/bin/breakpad_report.sh " + filename + " &");
		}catch(Exception e){
			e.printStackTrace();
			response.setStatus(404);
			response.sendError(404,"upload file err");
			
		}
		return null;
	}
	
	 synchronized long getTime(){
		return System.currentTimeMillis();
	}
	
}

package com.xiaonei.talk.gadget.cardinfo;

import java.io.OutputStream;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts.action.Action;
import org.apache.struts.action.ActionForm;
import org.apache.struts.action.ActionForward;
import org.apache.struts.action.ActionMapping;

public class CardInfoAction extends Action{

@Override
public ActionForward execute(ActionMapping mapping, ActionForm form,
		HttpServletRequest request, HttpServletResponse response)
		throws Exception {
	// TODO Auto-generated method stub
	int code = 0;
	try{
	String xml = CardInfoBuilder.buildXml(request);
	if(xml == null || xml.charAt(0) != '<'){
		System.err.println("xml null");
		response.setStatus(404);
		response.sendError(404,"null");
		
		if(xml == null){
			code = 6;
		}else{
			code = xml.charAt(0) - '0';
		}
		return null;
	}
	
	response.setContentType("text/xml");
	//response.setContentLength(xml.length());
	//response.setBufferSize(0);
	
	response.setDateHeader("Expires", System.currentTimeMillis()+10*60*1000);//设置过期时间
	OutputStream out = response.getOutputStream();
	out.write(xml.getBytes());
	out.flush();
	//response.flushBuffer();
	return null;
	}catch(Exception e){
		code = 7;
		throw e;
	}finally{
		if(code > 0){
			System.out.println("[STAT_ERR:"+String.valueOf(code)+"] get_card_info " + request.getRequestURI());
		}else{
			System.out.println("[STAT_SUCC:"+String.valueOf(code)+"] get_card_info "+ request.getRequestURI());
		}
	}
}

}

(function(){
	function logger()
	{
		this.el = document.createElement('div');
		this.el.className = 'debug';
		this.cnt = 0;
		with(this.el.style)
		{
			fontSize = '9pt';
			position = 'absolute';
			textAlign = 'left';
			top = '10px';
			left = '300px';
			width = '800px';
			height = '1000px';
			border = '1px solid';
			color = 'white';
			backgroundColor = '#000000';
			verticalAlign = 'bottom';
			overflow = 'auto';
		}
		//document.body.appendChild(this.el);
	}

	logger.prototype.hide = function()
	{
		this.el.style.display = 'none';	
		document.body.removeChild(this.el);
	}
	
	logger.prototype.reset = function()
	{
		this.cnt = 0;
		this.el.innerHTML = '';		
	}
	
	logger.prototype.println = function(str)
	{
		return;
		if(this.cnt ++ > 500)
		{
			this.cnt = 0;
			this.el.innerHTML = '';
		}
		var node = document.createTextNode(str);
		this.el.appendChild(node);
		node = document.createElement('br');
		this.el.appendChild(node);
		this.el.scrollTop = this.el.scrollHeight;
	}
	
	window.wplog = new logger();
})();


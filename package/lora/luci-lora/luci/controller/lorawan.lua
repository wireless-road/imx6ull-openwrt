module("luci.controller.lorawan", package.seeall)

function index()
	entry( {"admin", "lorawan"}, template("lorawan"), _("LoraWan"), 99 )
	entry( {"admin", "lorawan", "get_data"}, call("get_data")).leaf = true
	entry( {"admin", "lorawan", "set_data"}, post("set_data")).leaf = true
	entry( {"admin", "lorawan", "restart_lora"}, call("restart_lora")).leaf = true
end

require "nixio.fs"
require "luci.jsonc"

 dir = "/etc"

function get_data()
	local cfg, json_cfg
	local json_out = { }
	cfg = nixio.fs.readfile(dir .. "/global_conf.json", 524288)
	parser = luci.jsonc.new() 
	json_cfg = luci.jsonc.parse(cfg)

	json_out["gateway-id"] = json_cfg.gateway_conf.gateway_ID
	json_out["server-address"] = json_cfg.gateway_conf.server_address
	json_out["serv-port-up"] = json_cfg.gateway_conf.serv_port_up
	json_out["serv-port-down"] = json_cfg.gateway_conf.serv_port_down

	luci.http.prepare_content("application/json")
	luci.http.write_json(json_out or {})
	
end

function set_data()
	local cfg, json_cfg
	local json_str
	
	cfg = nixio.fs.readfile(dir .. "/global_conf.json", 524288)
	parser = luci.jsonc.new() 
	json_cfg = luci.jsonc.parse(cfg)
	
	local gw_id = luci.http.formvalue("gateway-id");
	local srv_addr = luci.http.formvalue("server-address")
	local srv_pup = luci.http.formvalue("serv-port-up")
	local srv_pdn = luci.http.formvalue("serv-port-down")
	
	if gw_id ~= nil and gw_id.len ~= nil then
		json_cfg.gateway_conf.gateway_ID = gw_id
	end
	if srv_addr ~= nil and srv_addr.len ~= nil then
		json_cfg.gateway_conf.server_address = srv_addr
	end
	if srv_pup ~= nil and srv_pup.len ~= nil then
		json_cfg.gateway_conf.serv_port_up = srv_pup
	end
	if srv_pdn ~= nil and srv_pdn.len ~= nil then
		json_cfg.gateway_conf.serv_port_down = srv_pdn
	end
		
	json_str = luci.jsonc.stringify(json_cfg, true)
	nixio.fs.writefile(dir .. "/global_conf.json", json_str)
	
	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.http.write("ok");
end

function restart_lora()
	luci.sys.exec("/etc/init.d/lora_pkt_fwd restart")

	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.http.write_json("ok");
end


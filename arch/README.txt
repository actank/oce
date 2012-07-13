客户端
 adapter.h 是独立的，只需头文件即可使用
 
 RegistryLocator
 

服务器端 service.h/cc
 添加服务 SERVANT_ENTRY，Creator/Destroyer
 启动后执行 AfterStartup
 box_main
 TODO: 支持在不同cc文件中加入服务

远程配置管理
 remoteconfig.ice

CHANGELOG:
 service 添加 AfterStartup
 Fusion rename to ClientPtr

TODO:
 logger.h IceLogger 好像没有生效
 Fusion::Reset
 支持replica的Fusion -- 清辉正在写




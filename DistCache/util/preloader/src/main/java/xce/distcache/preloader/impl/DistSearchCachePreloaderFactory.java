package xce.distcache.preloader.impl;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Map.Entry;

import com.xiaonei.xce.XceAdapter;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
//import oce.searchcache.DistSearchCache;
import xce.distcache.preloader.PreloaderFactory;
import xce.distcache.preloader.PreloaderLogger;
import xce.distcache.preloader.ProducerData;

public class DistSearchCachePreloaderFactory implements PreloaderFactory {
	public DistSearchCachePreloaderFactory() {
	}
	private List<String> SCBasicDataSources = new ArrayList<String>();
	private List<String> SCBasicTableNames = new ArrayList<String>();
	private List<ArrayList<String>> SCBasicFields = new ArrayList<ArrayList<String>>();
	private List<ArrayList<String>> SCBasicFieldsType = new ArrayList<ArrayList<String>>();
	private List<ArrayList<String>> SCBasicSetFunction = new ArrayList<ArrayList<String>>();
	private List<String> SCInfoDataSources = new ArrayList<String>();
	private List<String> SCInfoTableNames = new ArrayList<String>();
	private List<ArrayList<String>> SCInfoFields = new ArrayList<ArrayList<String>>();
	private List<ArrayList<String>> SCInfoFieldsType = new ArrayList<ArrayList<String>>();
	private List<ArrayList<String>> SCInfoSetFunction = new ArrayList<ArrayList<String>>();
	private List<String> SCInfoSet = new ArrayList<String>();

	void parseConfigFile(String fileName) {
		File file = new File(fileName);
		BufferedReader reader = null;
		try {
			System.out.println("以行为单位读取文件内容，一次读一整行：");
			reader = new BufferedReader(new FileReader(file));
			String tempString = null;
			int line = 0;
			int back_num = 0;

			boolean processType = true;

			while ((tempString = reader.readLine()) != null) {
				System.out.println("line " + line + ": " + tempString);
				if (back_num > 0) {
					back_num--;
					line++;
				} else {
					int index = tempString.indexOf("=") + 1;
					int end = tempString.indexOf(";");
					if (tempString.equals("required/optional"))
						continue;
					if (tempString.equals("repeated")) {
						processType = false;
					} else if (tempString.substring(0, 2).equals("TT")) {
						back_num = 2;
						line++;
					} else {
						switch (line % 3) {
						case 0:
							if (processType) {
								this.SCBasicDataSources.add(tempString
										.substring(index, end));
							} else {
								int index1 = tempString.indexOf(",");
								this.SCInfoDataSources.add(tempString
										.substring(index, index1));
								this.SCInfoSet.add(tempString.substring(
										index1 + 1, end));
							}
							break;
						case 1:
							if (processType)
								this.SCBasicTableNames.add(tempString
										.substring(index, end));
							else {
								this.SCInfoTableNames.add(tempString.substring(
										index, end));
							}
							break;
						case 2:
							List<String> field = new ArrayList<String>();
							List<String> fieldType = new ArrayList<String>();
							List<String> fieldFunc = new ArrayList<String>();
							int count = 0;
							while (index < end) {
								int index4 = tempString.indexOf(",", index);
								int index1 = tempString.indexOf(":", index);
								int index2 = tempString
										.indexOf(":", index1 + 1);
								int index3;
								if ((count == 0) && (line != 2))
									index3 = index2;
								else {
									index3 = tempString
											.indexOf(":", index2 + 1);
								}
								if (index4 == -1)
									index4 = end;
								String symbol = tempString.substring(
										index3 + 1, index4);
								String fi = tempString.substring(index, index1);
								String type = tempString.substring(index1 + 1,
										index2);
								String func = "";
								if (index3 > index2) {
									func = tempString.substring(index2 + 1,
											index3);
								}
								if ((symbol.equals("*"))
										|| (symbol.equals("1"))) {
									field.add(fi);
									fieldType.add(type);
									if (index3 > index2) {
										fieldFunc.add(func);
									}
								}
								index = index4 + 1;
								count++;
							}
							if (processType) {
								this.SCBasicFields
										.add((ArrayList<String>) field);
								this.SCBasicFieldsType
										.add((ArrayList<String>) fieldType);
								this.SCBasicSetFunction
										.add((ArrayList<String>) fieldFunc);
							} else {
								this.SCInfoFields
										.add((ArrayList<String>) field);
								this.SCInfoFieldsType
										.add((ArrayList<String>) fieldType);
								this.SCInfoSetFunction
										.add((ArrayList<String>) fieldFunc);
							}
						}
						line++;
					}
				}
			}
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();

			if (reader != null)
				try {
					reader.close();
				} catch (IOException localIOException1) {
				}
		} finally {
			if (reader != null)
				try {
					reader.close();
				} catch (IOException localIOException2) {
				}
		}

	}

	void initialize() {
		String configFile = "../etc/Database.conf";
		parseConfigFile(configFile);
	}

	public Map<Long, byte[]> CreateFromDB(ProducerData data) {
		PreloaderLogger
				.debug("DistSearchCachePreloaderFactory.CreateFromDB, ids.size(): "
						+ data.getIds().size());
		/*
		Map<Long, DistSearchCache.DistSearchCacheData.Builder> resultTmp = new HashMap<Long, DistSearchCache.DistSearchCacheData.Builder>();
        
		for(int i = 0; i < SCBasicDataSources.size(); i++)  {
			try {
				Connection c = XceAdapter.getInstance().getReadConnection(SCBasicDataSources.get(i));
				try {
					Statement s = c.createStatement();
					try  {
						String field = "";
						int count = 0;
						for(String str : SCBasicFields.get(i))  {
						  field += str;
						  if(count != SCBasicFields.size() - 1)  {
							  field += ",";
						  }
						}
						String sql = "SELECT " + field + " FROM " + SCBasicTableNames.get(i) +
						          " WHRER id > " + data.getMin() + " AND id <=" + data.getMax();
						ResultSet sqlSet = s.executeQuery(sql);
						try  {
							int j = 0;
              DistSearchCache.DistSearchCacheData.Builder search_cache_data_builder = null;
							while(sqlSet.next())  {
							  if(j == 0)  {
								if(i == 0)  {
									search_cache_data_builder = DistSearchCache.DistSearchCacheData.newBuilder();
									Long id = sqlSet.getLong(SCBasicFields.get(i).get(j));
									//search_cache_data_builder.getClass().getMethod(SCBasicSetFunction.get(i).get(j), new Class[] { Long.class })
									//.invoke(search_cache_data_builder, new Object[] {id});
									continue;
								}  else  {
									Long id = sqlSet.getLong(SCBasicFields.get(i).get(j));
								   search_cache_data_builder = resultTmp
								    .get(id);
									continue;
								}
							  }
							  if(search_cache_data_builder != null)  {
								  if(SCBasicFieldsType.get(i).get(j).equals("long"))  {
									  Long value = sqlSet.getLong(SCBasicFields.get(i).get(j));
									  search_cache_data_builder.getClass().getMethod(SCBasicSetFunction.get(i).get(j), new Class[] { Long.class})
									  .invoke(search_cache_data_builder, new Object[] { value });
								  }  else if(SCBasicFieldsType.get(i).get(j).equals("int"))  {
									  int value = sqlSet.getInt(SCBasicFields.get(i).get(j));
									  search_cache_data_builder.getClass().getMethod(SCBasicSetFunction.get(i).get(j), new Class[] {Integer.class})
                                      .invoke(search_cache_data_builder, new Object[] {value});
								  }  else if(SCBasicFieldsType.get(i).get(j).equals("string"))  {
									  String value = sqlSet.getString(SCBasicFields.get(i).get(j));
									  search_cache_data_builder.getClass().getMethod(SCBasicSetFunction.get(i).get(j), new Class[] {String.class})
									  .invoke(search_cache_data_builder, new Object[] {value});
								  }
							  }
							  j++;
							}
						}  catch (Exception e) {
							PreloaderLogger.warn(e);
						} finally {
							sqlSet.close();
						}
					} catch (Exception e) {
						PreloaderLogger.warn(e);
					} finally {
						s.close();
					}
				} catch (Exception e) {
					PreloaderLogger.warn(e);
				} finally {
					c.close();
				}
			} catch (Exception e) {
				PreloaderLogger.warn(e);
			}
		}
		
		for(int i = 0; i < SCInfoDataSources.size(); i++)  {
			try {
				Connection c = XceAdapter.getInstance().getReadConnection(SCInfoDataSources.get(i));
				try {
					Statement s = c.createStatement();
					try  {
						String field = "";
						int count = 0;
						for(String str : SCInfoFields.get(i))  {
						  field += str;
						  if(count != SCInfoFields.size() - 1)  {
							  field += ",";
						  }
						}
						String sql = "SELECT " + field + " FROM " + SCInfoTableNames.get(i) +
						          " WHRER id > " + data.getMin() + " AND id <=" + data.getMax();
						ResultSet sqlSet = s.executeQuery(sql);
						try  {
							int j = 0;
              DistSearchCache.DistSearchCacheData.Builder search_cache_data_builder = null;
							while(sqlSet.next())  {
							  if(j == 0)  {
									Long id = sqlSet.getLong(SCInfoFields.get(i).get(j));
									search_cache_data_builder = resultTmp
								    .get(id);
									continue;
							  }
							  if(search_cache_data_builder != null)  {
									  String ClassName = "oce.searchcache.DistSearchCache.DistSearchCacheData."+SCInfoSet.get(i)+".Builder";
									  if(SCInfoFieldsType.get(i).get(j).equals("int"))  {
										  int value = sqlSet.getInt(SCInfoFields.get(i).get(j));
										  Object instance = Class.forName(ClassName).newInstance();
										  instance.getClass().getMethod(SCInfoSetFunction.get(i).get(j), new Class[] {Integer.class})
	                                      .invoke(instance, new Object[] {value});
										  search_cache_data_builder.getClass().getMethod("add"+SCInfoSet.get(i), new Class[] {Class.forName(ClassName)})
                                          .invoke(search_cache_data_builder, new Object[] {value});
									  }  else if(SCInfoFieldsType.equals("string"))  {
										  String value = sqlSet.getString(SCInfoFields.get(i).get(j));
										  Object instance = Class.forName(ClassName).newInstance();
										  instance.getClass().getMethod(SCInfoSetFunction.get(i).get(j), new Class[] {String.class})
	                                      .invoke(instance, new Object[] {value});
										  search_cache_data_builder.getClass().getMethod("add"+SCInfoSet.get(i), new Class[] {Class.forName(ClassName)})
                                          .invoke(search_cache_data_builder, new Object[] {value});
									  }
									  
								  }
							  j++;
							}
						}  catch (Exception e) {
							PreloaderLogger.warn(e);
						} finally {
							sqlSet.close();
						}
					} catch (Exception e) {
						PreloaderLogger.warn(e);
					} finally {
						s.close();
					}
				} catch (Exception e) {
					PreloaderLogger.warn(e);
				} finally {
					c.close();
				}
			} catch (Exception e) {
				PreloaderLogger.warn(e);
			}
		}
		
		PreloaderLogger.debug("DistSearchCachePreloaderFactory.CreateFromDB ,size: "
				+ resultTmp.size());
		Map<Long, byte[]> result = new TreeMap<Long, byte[]>();
		for (Entry<Long, DistSearchCache.DistSearchCacheData.Builder> entry : resultTmp
				.entrySet()) {

			try {
				result.put(entry.getKey(), entry.getValue().build()
						.toByteArray());
			} catch (Exception e) {
				PreloaderLogger
						.warn("DistSearchCacheData.build().toByteArray, Exception: "
								+ e.toString());
			}
		}*/
		return null;
	}

	public Map<Long, byte[]> CreateFromNode(List<Long> ids) {
		return null;
	}

	public Map<Long, byte[]> CreateFromTT(ProducerData data) {
		return null;
	}
}

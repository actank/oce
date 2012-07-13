-- MySQL dump 10.13  Distrib 5.1.31, for debian-linux-gnu (i486)
--
-- Host: 10.2.18.202    Database: beijing
-- ------------------------------------------------------
-- Server version	5.1.38

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `instance`
--

DROP TABLE IF EXISTS `instance`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `instance` (
  `name` varchar(255) NOT NULL PRIMARY KEY,
  `type` varchar(20) NOT NULL,
  `timestamp` bigint(21) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `instance`
--

LOCK TABLES `instance` WRITE;
/*!40000 ALTER TABLE `instance` DISABLE KEYS */;
INSERT INTO `instance` VALUES ('common','singler',12345678),('user_network','singler',23456789),('user_common','singler',22345678),('xfeed_news','router',33456789),('xfeed_news_0_9','singler',42345678),('xfeed_news_10_19','singler',53456789);
/*!40000 ALTER TABLE `instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `route`
--

DROP TABLE IF EXISTS `route`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `route` (
  `id`       bigint AUTO_INCREMENT PRIMARY KEY,
  `instance_name` varchar(255) NOT NULL,
  `expression` varchar(255) NOT NULL,
  `instance` varchar(255) NOT NULL,
  KEY `R_5` (`instance_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `route`
--

LOCK TABLES `route` WRITE;
/*!40000 ALTER TABLE `route` DISABLE KEYS */;
INSERT INTO `route` VALUES (null,'xfeed_news','xfeed_news_[0-9]{1}','xfeed_news_0_9'),(null,'xfeed_news','xfeed_news_[1]{1}[0-9]{1}','xfeed_news_10_19');
/*!40000 ALTER TABLE `route` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `server`
--

DROP TABLE IF EXISTS `server`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `server` (
  `id`   bigint      AUTO_INCREMENT PRIMARY KEY,
  `type` varchar(20) NOT NULL,
  `database` varchar(40) NOT NULL,
  `host` varchar(20) NOT NULL,
  `port` int(11) NOT NULL,
  `user` varchar(20) NOT NULL,
  `password` varchar(20) NOT NULL,
  `wrflag` varchar(20) NOT NULL,
  `charset` varchar(20) DEFAULT NULL,
  `priority` varchar(20) DEFAULT NULL,
  `instance_name` varchar(255) NOT NULL,
  KEY `R_6` (`instance_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `server`
--

LOCK TABLES `server` WRITE;
/*!40000 ALTER TABLE `server` DISABLE KEYS */;
INSERT INTO `server` VALUES (null,'mysql','fb','user1.db',3306,'xndev','pwd','w','utf8','','common'),(null,'mysql','fb','user1.db',3306,'xndev','pwd','r','utf8','','common'),(null,'mysql','fb','user1.db',3306,'xndev','pwd','r','utf8','','common'),(null,'mysql','fb','user3.db',3306,'xndev','pwd','r','utf8','','common'),(null,'mysql','fb','user5.db',3306,'xndev','pwd','r','utf8','','common'),(null,'mysql','fb','user1.db',3306,'xndev','pwd1','w','utf8','','user_network'),(null,'mysql','fb','user5.db',3306,'xndev','pwd1','r','utf8','','user_network'),(null,'mysql','fb','user5.db',3306,'xndev','pwd1','r','utf8','','user_network'),(null,'mysql','fb','user8.db',3306,'xndev','pwd3','w','utf8','','user_common'),(null,'mysql','fb','user9.db',3306,'xndev','pwd3','r','utf8','','user_common'),(null,'mysql','fb','user9.db',3306,'xndev','pwd3','r','utf8','','user_common'),(null,'mysql','xfeed','feed11.db',3306,'xndev','pwd5','w','utf8','','xfeed_news_0_9'),(null,'mysql','xfeed','feed11.db',3306,'xndev','pwd5','w','utf8','','xfeed_news_10_19'),(null,'mysql','xfeed','feed11.db',3306,'xndev','pwd5','r','utf8','','xfeed_news_10_19'),(null,'mysql','xfeed','feed11.db',3306,'xndev','pwd5','r','utf8','','xfeed_news_10_19');
/*!40000 ALTER TABLE `server` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ufo`
--

DROP TABLE IF EXISTS `ufo`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ufo` (
  `id` varchar(20) NOT NULL,
  `addr` varchar(100) NOT NULL,
  `age` int(11) DEFAULT '0',
  `name` char(20) NOT NULL DEFAULT 'm',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ufo`
--

LOCK TABLES `ufo` WRITE;
/*!40000 ALTER TABLE `ufo` DISABLE KEYS */;
/*!40000 ALTER TABLE `ufo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-09-11  3:48:07

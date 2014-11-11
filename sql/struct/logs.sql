/*
SQLyog Community v11.3 (64 bit)
MySQL - 5.6.14-62.0 : Database - logs
*********************************************************************
*/


/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*Table structure for table `chat_logs` */

DROP TABLE IF EXISTS `chat_logs`;

CREATE TABLE `chat_logs` (
  `sender_accid` int(11) NOT NULL DEFAULT '0',
  `sender_guid` int(11) NOT NULL DEFAULT '0',
  `text` varchar(255) NOT NULL,
  `receiver_accid` int(11) NOT NULL DEFAULT '0',
  `receiver_guid` int(11) NOT NULL DEFAULT '0',
  `type` int(3) NOT NULL DEFAULT '0',
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Data for the table `chat_logs` */

/*Table structure for table `command_logs` */

DROP TABLE IF EXISTS `command_logs`;

CREATE TABLE `command_logs` (
  `sender_accid` int(11) NOT NULL DEFAULT '0',
  `sender_guid` int(11) NOT NULL DEFAULT '0',
  `command` varchar(255) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `target_accid` int(11) NOT NULL DEFAULT '0',
  `target_guid` int(11) NOT NULL DEFAULT '0',
  `target_typeid` int(1) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Table structure for table `error_logs` */

DROP TABLE IF EXISTS `error_logs`;

CREATE TABLE `error_logs` (
  `type` tinyint(2) NOT NULL,
  `string` text NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*Table structure for table `trade_logs` */

DROP TABLE IF EXISTS `trade_logs`;

CREATE TABLE `trade_logs` (
  `trader_accid` int(11) NOT NULL,
  `trader_guid` int(11) NOT NULL,
  `item_id` int(10) NOT NULL,
  `item_count` int(10) NOT NULL,
  `receiver_accid` int(11) NOT NULL,
  `receiver_guid` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

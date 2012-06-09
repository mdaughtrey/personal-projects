#!/usr/bin/python

import sys
import pdb
import argparse
import ConfigParser
import logging
import string
import MetarIngest

configFilename = 'webingest.cfg'
#logLevelMap = { 'debug' : logging.DEBUG,
#    'info' : logging.INFO,
#    'warning' : logging.WARNING,
#    'error' : logging.ERROR,
#    'critical' : logging.CRITICAL
#}


def main():
    #
    # Read the configuration file
    #
    config = ConfigParser.SafeConfigParser()
    config.read(configFilename)

    #
    # Read the command line and override any config file operations
    #
    parser = argparse.ArgumentParser(
            description = 'Read station data from the web')
    parser.add_argument('--verbosity', '-v',
            help = 'debug|info|warning|error|critical',
            default = config.get('logging', 'threshold', None))
    parser.add_argument('--skipdb', '-s',
            action = 'store_true',
            help = 'skip database update')
    args = parser.parse_args()

    config.set('logging', 'threshold', args.verbosity) 
    config.set('debug', 'skipdb', str(args.skipdb))

    #
    # Set up the logger
    #
    try:
        logging.basicConfig(
                level = getattr(
                    logging,
                    config.get('logging', 'threshold').upper(),
                    None),
                format = '%(asctime)s %(levelname)s %(name)s %(message)s',
                datefmt = '%Y%d%m %H:%M:%S')
        log = logging.getLogger('webingest')

    except KeyError:
        print "Unknown logging threshold [%s]" % config.get('logging', 'threshold')
        sys.exit(1)

    log.info('Ready')

    mi = MetarIngest.Ingest(config)
    if False == mi():
        log.error("METARS retrieval failed")

    # read stations.txt
    # for each stations.txt read METAR and add to database

main()


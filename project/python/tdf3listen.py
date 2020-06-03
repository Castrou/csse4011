#!/usr/bin/env python3
"""
TDF Listener that outputs all TDFs recieved to the screen
"""
__author__ = "Jordan Yates"

import argparse
import datetime

import tdf3
import ListenClient

from PacpMessage import PayloadType, DecryptionError
from NodeFilter import NodeFilter

from basehost import Node, staticNode, mobileNode, usStaticNode

class tdf3listen:
    def __init__(self, base_host, base_port, tdf_server):
        self.base_host = base_host
        self.base_port = base_port
        self.tdf_server = tdf_server
        self.filter_config = None
        self.filter_strict = 0
        self.show_raw = 0
        self.debug = 0
        self.mobile = dict()
        self.static = dict()
        self.heading = 0
        self.steps = 0

    def run(self):
        tdfparse = tdf3.Tdf()
        tdfparse.loadTdf(self.tdf_server, timeout=10.0)
        parser = tdfparse.parseTdf16

        listener = ListenClient.ListenClient(self.base_host, self.base_port)
        listener.connect()
        node_filter = NodeFilter(self.filter_config)

        if self.show_raw:
            template = "{:17}| {:>7} | {:24} | {:>20} | {:10}| {:}"  # column widths: 24 - date, 17 - node mac, 7 - rssi, 20 - value, 10 - value raw,  35 - phenomenon
        else:
            template = "{:17}| {:>7} | {:24} | {:>20} | {:}"  # column widths: 24 - date, 17 - node mac, 7 - rssi, 20 - value, 35 - phenomenon

        try:
            while True:
                try:
                    packet = listener.read(timeout=None)
                except ConnectionResetError:
                    print("Connection to baselisten lost...")
                    return
                except NotImplementedError:
                    continue 

                for payload_type, route, payload in packet.iter_payloads():

                    if payload_type == PayloadType.PAYLOAD_ENC_TDF3:
                        # Only try if there are encryption key
                        if node_filter.encryption_keys == []:
                            continue
                        # Attempt decryption using filter keys
                        decrypted = False
                        for key in node_filter.encryption_keys:
                            try:
                                route, payload = packet.decrypt(route, payload, key)
                            except DecryptionError:
                                continue
                            decrypted = True
                            break
                        if not decrypted:
                            continue
                    elif payload_type != PayloadType.PAYLOAD_TDF3:
                        continue

                    first_hop = route[-1]
                    pkt_addr = first_hop.address_fmt
                    pkt_rssi = "{:d}dBm".format( first_hop.rssi)
                    first_tdf = True

                    try:
                        for point in parser(payload, datetime.datetime.utcnow(), debug=self.debug, combine=True):
                            # Apply device filtering rules
                            node_filter.consume_tdf(first_hop.address, point)
                            self.filter_result = node_filter.filter(first_hop.address)
                            if self.filter_strict and self.filter_result != NodeFilter.PASSED:
                                continue
                            elif self.filter_result == NodeFilter.FAILED:
                                continue

                            # Newline between Packets
                            if first_tdf:
                                # print("")
                                # if self.show_raw:
                                #     print(template.format("Node", first_hop.interface, "Date", "Value", "Raw", "Phenomenon"))
                                # else:
                                #     print(template.format("Node", first_hop.interface, "Date", "Value", "Phenomenon"))
                                first_tdf = False

                            pkt_time = point['time'].strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                            ##################################################################
                            RxAddr = str(pkt_addr)
                            # if RxAddr in self.mobile:
                            #     pass
                            # else:
                            self.mobile[RxAddr] = mobileNode(RxAddr)
                            if (point['sensor'] == 'LSM6DSL'):
                                # Node & Type
                                nodeType = (point['phenomena']['acc_x']['raw'] >> 8)
                                nodeId = (point['phenomena']['acc_x']['raw'] & 0xFF) >> 4
                                nodeName = "Node "+ str(nodeId)
                                # mm Dist
                                mmDist = point['phenomena']['gyro_x']['raw']
                                self.heading = point['phenomena']['gyro_z']['raw']
                                self.steps = point['phenomena']['acc_z']['raw']
                                ## Static / Ultrasonic Split ##
                                if (nodeType == 1):
                                    # X/Y
                                    x_pos = (point['phenomena']['acc_y']['raw'] & 0xFF) * 100
                                    y_pos = (point['phenomena']['acc_y']['raw'] >> 8) * 100
                                    if nodeName in self.static:
                                        self.static[nodeName].updateInfo(x_pos, y_pos, mmDist)
                                    else:
                                        self.static[nodeName] = staticNode(nodeName, x_pos, y_pos, mmDist)
                                elif (nodeType == 2):
                                    # X/Y
                                    x_pos = (point['phenomena']['acc_y']['raw'] & 0xFF) * 100
                                    y_pos = (point['phenomena']['acc_y']['raw'] >> 8) * 100
                                    # Ultrasonic
                                    usVal = point['phenomena']['gyro_y']['raw']
                            ##################################################################
                            for name, p in point['phenomena'].items():                                
                                # if self.show_raw:
                                    # print(template.format(pkt_addr, pkt_rssi, pkt_time, p['formatted'], p['raw'], "{:s}_{:s}".format(point['sensor'], name)))
                                # else:
                                    # print(template.format(pkt_addr, pkt_rssi, pkt_time, p['formatted'], "{:s}_{:s}".format(point['sensor'], name)))
                                pkt_rssi = ""
                                pkt_time = ""
                                pkt_addr = ""
                    except tdf3.TdfBufferLengthError as e:
                        print("Node: {:} = {}".format(first_hop.address_fmt, e))
                    except tdf3.TdfLookupError as e:
                        print("Node: {:} = {}".format(first_hop.address_fmt, e))

        except KeyboardInterrupt:
            pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='TDF listener')
    parser.add_argument('--host', dest='base_host', type=str, default="localhost", help='Hostname for baselisten')
    parser.add_argument('--port', dest='base_port', type=int, default=9001, help='Port for baselisten')
    parser.add_argument('--tdf', dest='tdf_server', type=str, default=None, help='Hostname for TDF server')

    display_args = parser.add_argument_group("Display controls")
    display_args.add_argument('--filter', dest='filter_config', type=str, default=None, help='Filter configuration file')
    display_args.add_argument('--strict', dest='filter_strict', action="store_true", help="Don't display unsure devices")
    display_args.add_argument('--raw', dest='show_raw', action="store_true", help='Show raw phenomenon values')
    display_args.add_argument('--debug', dest='debug', action="store_true", help='Enable debug for TDF decoding')

    args = parser.parse_args()

    tdf3listen(**vars(args))


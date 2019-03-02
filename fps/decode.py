#!/usr/bin/env python3

import binascii
import datetime
import struct

class Time:
    def __init__(self, year, month, day, hour, minute, second):
        self.year = year
        self.month = month
        self.day = day
        self.hour = hour
        self.minute = minute
        self.second = second

    def __str__(self):
        return '{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}'.format(
            self.year, self.month, self.day,
            self.hour, self.minute, self.second,
        )

    def pack(self):
        return struct.pack('>HBBBBB', self.year, self.month, self.day, self.hour, self.minute, self.second)

    def unix(self):
        return datetime.datetime.Datetime(self.year, self.month, self.day, self.hour, self.minute, self.second).strftime("%s")


class Fix:
    def __init__(self, quality, sats):
        self.sats = sats
        self.quality = quality

    def __str__(self):
        return 'quality: {} satellites: {}'.format(self.quality, self.sats)


def latitude(lat):
    if lat >= 0.0:
        return '{:0.6f} N'.format(lat)
    else:
        return '{:0.6f} S'.format(lat)


def longitude(lon):
    if lon >= 0.0:
        return '{:0.6f} E'.format(lon)
    else:
        return '{:0.6f} W'.format(lon)


class Position:
    def __init__(self, lat, lon, year, month, day, hour, minute, second, quality, sats):
        self.lat = lat
        self.lon = lon
        self.time = Time(year, month, day, hour, minute, second)
        self.fix = Fix(quality, sats)

    def __str__(self):
        return 'GPS position:\n\tPOS: {}, {}\n\tFIX: {}\n\tTIME: {}\n'.format(
                latitude(self.lat), longitude(self.lon),
                self.fix, self.time)


def decode(packet):
    packet = packet.replace(' ', '')
    packet = binascii.unhexlify(packet)
    pos = struct.unpack('<ffHBBBBBxBBxx', packet)
    return Position(pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6],
                    pos[7], pos[8], pos[9])


TESTDATA = '5F566A45AFE43E46E30702191700280001080000'
def test():
    pos = decode('5F566A45AFE43E46E30702191700280001080000')
    print(pos)
    return pos

test()

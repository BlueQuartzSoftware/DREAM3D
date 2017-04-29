#!/usr/bin/env python
import os
import hashlib
import requests
import subprocess
import sys

downloads = [{
    '_id': '58a20cdb8d777f0721a48b8c',
    'name': 'boost-1.60.0.7z',
    'target': '.',
}, {
    '_id': '58a220818d777f0721a4cc4a',
    'name': 'doxygen-1.8.11.windows.x64.bin.7z',
    'target': '.',
}, {
    '_id': '58a2ff998d777f0721a57c3c',
    'name': 'Eigen-3.2.9.7z',
    'target': '.',
}, {
    '_id': '58a3436c8d777f0721a61036',
    'name': 'hdf5-1.8.16.7z',
    'target': '.',
}, {
    '_id': '58a3819a8d777f0721a659ea',
    'name': 'ITK-4.11.0.7z',
    'target': '.',
}, {
    '_id': '58a3819a8d777f0721a659ea',
    'name': 'qwt-6.1.3.7z',
    'target': '.',
}, {
    '_id': '58a3819a8d777f0721a659ea',
    'name': 'tbb44_20160526oss.7z',
    'target': '.',
}, {
    '_id': '58a3819a8d777f0721a659ea',
    'name': 'superbuild.7z',
    'target': '.',
}
]

girder_url = 'http://dream3d.bluequartz.net/binaries/appveyor'


def is_cached(id):
    sha_path = '%s.sha512' % download['name']
    if os.path.exists(sha_path):
        url = '%s/%s.sha512.txt' % (girder_url, download['name'])
        response = requests.get(url)
        sha = response.content

        with open(sha_path) as fp:
            cached_sha = fp.read()

        return sha == cached_sha

    return False


def extract(download):
    target = download['target']
    name = download['name']

    print('Extracting %s to %s ...' % (name, target))
    if not os.path.exists(target):
        os.makedirs(target)

    # Call out to 7z, zipfile seems to be very slow for this sort of network
    # filesystem.
    cmd = ['7z', 'x', name, '-o%s' % target]
    print('Extract command: %s' % cmd)
    process = subprocess.Popen(cmd)
    process.wait()
    if process.returncode != 0:
        sys.exit(process.returncode)


CHUNK_SIZE = 16 * 1024


def cache_download(download):
    url = '%s/%s' % (girder_url, download['name'])
    response = requests.get(url, stream=True)

    sha = hashlib.sha512()
    name = download['name']
    print('Downloading %s ...' % name)
    with open(name, "wb") as fp:
        for chunk in response.iter_content(chunk_size=CHUNK_SIZE):
            if chunk:
                fp.write(chunk)
                sha.update(chunk)

    sha_path = '%s.sha512' % download['name']
    print('Writing SHA512 to %s ...' % sha_path)

    with open(sha_path, 'w') as sha_fp:
        sha_fp.write(sha.hexdigest())


for download in downloads:

    if not is_cached(download['_id']):
        cache_download(download)
    else:
        print('Using cache for %s' % download['name'])

    extract(download)
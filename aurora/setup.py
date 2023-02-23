from setuptools import setup

setup(name='aurora_bs',
      version='0.1',
      description='The Aurora Build System',
      author='Corwin McKnight',
      author_email='corwin.mcknight@gmail.com',
      license='MIT',
      packages=['aurora'],
      scripts=['bin/aur', 'bin/aurora_build'],
      zip_safe=False)
from setuptools import setup, find_packages

# Call the setup function to specify package details and configuration
setup(
    # Name of the package
    name='heat-equation-solver',

    # Version of the package
    version='0.1',

    # Automatically find all packages and subpackages in the 'src' directory
    packages=find_packages(where='src'),

    # Specify that the root package is in the 'src' directory
    package_dir={'': 'src'},

    # List of dependencies to be installed along with the package
    install_requires=[
        'numpy',  # NumPy library for numerical computations
    ],

    # Directory where test cases are located
    test_suite='tests',
)

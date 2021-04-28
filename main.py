import argparse
import sys, os
class MainArgParser:
    def __init__(self):
        parser = argparse.ArgumentParser(prog='balance')
        parser.add_argument(dest='subfunc', help='Subcommands: ')
        args = parser.parse_args(sys.argv[1:2])
        getattr(self, args.subfunc)()
    def parseILP(self):
        import parseILP
        parser = argparse.ArgumentParser(description='Parse ILP results')
        parser.add_argument('-i', '--in-checked-lh',
                            dest='checked_lh',
                            required=True,
                            help='Checked lh file')
        parser.add_argument('-s', '--sol',
                            dest='sol_file',
                            required=True,
                            help='Solution file')
        parser.add_argument('-o', '--output-balanced-lh',
                            dest='balanced_lh',
                            required=True,
                            help='Output balanced lh file')
        args = parser.parse_args(sys.argv[2:])
        sol = parseILP.parse_ilp_result(args.sol_file)
        parseILP.generate_balanced_lh(args.balanced_lh, args.checked_lh, sol)
if __name__ == '__main__':
    MainArgParser()

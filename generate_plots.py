"""Generate report-ready plots from the supplied miss rates (percent).

Run: python3 plot_hawkeye.py
Requires: matplotlib. Outputs go into plots/ beside this script.
These values are user-supplied; this script does not validate simulator logs.
"""
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

WAYS = [4, 8, 16]
LRU_HMMER = [33.8871, 26.9257, 25.9442]
HAWKEYE_HMMER = [35.3654, 26.9215, 24.2202]
BENCHMARKS = ["456.hmmer-191B", "429.mcf-22B", "473.astar-42B"]
LRU = [25.9442, 64.8190, 19.6324]
HAWKEYE = [24.2202, 63.7818, 19.2289]
AUTHOR = "Dhairya Aundhia"  # Use "" to omit the author.
OUT = Path(__file__).resolve().parent / "plots"
BLUE, ORANGE = "#0072B2", "#D55E00"


def canvas(title, subtitle):
    fig, ax = plt.subplots(figsize=(12, 7.5))
    fig.subplots_adjust(left=0.12, right=0.97, bottom=0.19, top=0.81)
    fig.text(0.12, 0.94, title, fontsize=22, weight="bold", ha="left")
    fig.text(0.12, 0.885, subtitle, fontsize=13, color="#505865")
    ax.spines[["top", "right"]].set_visible(False)
    for side in ["left", "bottom"]:
        ax.spines[side].set_color("#A0A7B0")
    ax.set_axisbelow(True)
    ax.grid(axis="y", color="#E1E5EA", linewidth=0.9)
    ax.tick_params(labelsize=12, length=0, pad=9)
    if AUTHOR:
        fig.text(0.97, 0.035, AUTHOR, ha="right", fontsize=10, color="#626A76")
    return fig, ax


def save(fig, name):
    for extension in ["png", "svg"]:
        fig.savefig(OUT / f"{name}.{extension}", dpi=300, facecolor="white")
    plt.close(fig)


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    plt.rcParams.update({"font.family": "DejaVu Sans", "axes.labelsize": 14,
                         "axes.labelpad": 12, "svg.fonttype": "none"})
    assert LRU_HMMER[-1] == LRU[0] and HAWKEYE_HMMER[-1] == HAWKEYE[0]
    assert len(LRU) == len(HAWKEYE) == len(BENCHMARKS)
    assert all(v > 0 for v in LRU)

    fig, ax = canvas("LLC miss rate vs. associativity", "456.hmmer-191B  |  Lower miss rate is better")
    ax.plot(WAYS, LRU_HMMER, color=BLUE, marker="o", markersize=9,
            markerfacecolor="white", markeredgewidth=2.3, linewidth=2.6, label="LRU")
    ax.plot(WAYS, HAWKEYE_HMMER, color=ORANGE, marker="s", markersize=7,
            linewidth=2.6, linestyle="--", label="Hawkeye")
    ax.set(xlabel="LLC associativity (ways)", ylabel="LLC miss rate (%)",
           xticks=WAYS, xlim=(3, 17.2), ylim=(23, 37))
    ax.legend(loc="upper right", frameon=False, fontsize=13)
    for values, color, offsets in [
        (LRU_HMMER, BLUE, [(12, -22), (-20, 24), (-10, 16)]),
        (HAWKEYE_HMMER, ORANGE, [(12, 15), (22, -35), (-10, -15)]),
    ]:
        for i, (x, y, offset) in enumerate(zip(WAYS, values, offsets)):
            # Extra precision exposes the near-tie at 8 ways.
            text = f"{y:.4f}%" if i == 1 else f"{y:.2f}%"
            ax.annotate(text, (x, y), xytext=offset, textcoords="offset points",
                        ha="right" if offset[0] < 0 else "left", va="center",
                        color=color, fontsize=12, weight="bold",
                        bbox=dict(facecolor="white", edgecolor="none", alpha=0.9, pad=2),
                        arrowprops=dict(arrowstyle="-", color=color, lw=0.8) if i == 1 else None)
    fig.text(0.12, 0.065, "8-way difference: 0.0042 percentage points (nearly tied).",
             fontsize=11, color="#505865")
    save(fig, "plot1_hmmer_associativity")

    reductions = [(l - h) / l * 100 for l, h in zip(LRU, HAWKEYE)]
    fig, ax = canvas("Hawkeye LLC miss-rate reduction over LRU",
                     "2 MB LLC, 16-way  |  Positive values indicate lower miss rates")
    bars = ax.bar(range(len(BENCHMARKS)), reductions, width=0.56,
                  color=[BLUE if value >= 0 else ORANGE for value in reductions])
    padding = max(max(abs(v) for v in reductions) * 0.2, 0.5)
    ax.set_ylim(min(0, min(reductions) - padding) if min(reductions) < 0 else 0,
                max(0, max(reductions) + padding))
    ax.set_xticks(range(len(BENCHMARKS)), BENCHMARKS)
    ax.set_xlabel("Benchmark")
    ax.set_ylabel("LLC miss-rate reduction over LRU (%)")
    ax.axhline(0, color="#525B66", linewidth=1)
    for bar, value in zip(bars, reductions):
        ax.annotate(f"{value:+.2f}%", (bar.get_x() + bar.get_width()/2, value),
                    xytext=(0, 9 if value >= 0 else -9), textcoords="offset points",
                    ha="center", va="bottom" if value >= 0 else "top",
                    fontsize=16, weight="bold", color="#17202A")
    fig.text(0.12, 0.065, "Reduction = (LRU miss rate - Hawkeye miss rate) / LRU miss rate × 100",
             fontsize=11, color="#505865")
    save(fig, "plot2_miss_rate_reduction")
    print("Miss-rate reductions (calculated before display rounding):")
    for benchmark, value in zip(BENCHMARKS, reductions):
        print(f"  {benchmark}: {value:+.6f}%")
    print(f"PNG and SVG plots saved in: {OUT}")


if __name__ == "__main__":
    main()

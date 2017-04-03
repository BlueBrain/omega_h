#include "Omega_h.hpp"
#include "Omega_h_compare.hpp"

int main(int argc, char** argv) {
  auto lib = Omega_h::Library(&argc, &argv);
  OMEGA_H_CHECK(argc == 2);
  auto inpath = argv[1];
  auto mesh = Omega_h::Mesh(&lib);
  auto maximum_size = Omega_h::Real(0.9);
  auto target_error = Omega_h::Real(0.011);
  auto gradation_rate = Omega_h::Real(1.0);
  auto max_metric_length = Omega_h::Real(2.8);
  Omega_h::binary::read(inpath, lib.world(), &mesh);
  mesh.balance();
  mesh.reorder();
  mesh.set_parting(OMEGA_H_GHOSTED);
  auto genopts = Omega_h::MetricInput();
  genopts.sources.push_back({OMEGA_H_HESSIAN, OMEGA_H_SCALES, "Solution", target_error});
  genopts.should_limit_lengths = true;
  genopts.min_length = 0.0;
  genopts.max_length = maximum_size;
  genopts.should_limit_gradation = true;
  genopts.max_gradation_rate = gradation_rate;
  Omega_h::generate_target_metric_tag(&mesh, genopts);
  Omega_h::add_implied_metric_tag(&mesh);
  Omega_h::AdaptOpts opts(&mesh);
  opts.xfer_opts.type_map["Solution"] = OMEGA_H_LINEAR_INTERP;
  opts.max_length_allowed = max_metric_length;
  while (Omega_h::approach_metric(&mesh, opts)) {
    Omega_h::adapt(&mesh, opts);
  }
  bool ok = check_regression("gold_advect2d", &mesh);
  if (!ok) return 2;
  return 0;
}

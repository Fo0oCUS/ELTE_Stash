<script setup lang="ts">
import { useRouter } from 'vue-router'
import { useUserStore } from '../stores/userStore';

const userStore = useUserStore();
const router = useRouter();

const handleLogout = () => {
  userStore.logout();
  router.push("/");
}
</script>

<template>
  <nav class="navbar navbar-expand-lg navbar-dark bg-dark shadow-sm py-3">
    <div class="container">
      <RouterLink class="navbar-brand fw-bold fs-4" to="/">
        MWF - Quiz
      </RouterLink>

      <button
        class="navbar-toggler border-0 shadow-none"
        type="button"
        data-bs-toggle="collapse"
        data-bs-target="#navbarNav"
      >
        <span class="navbar-toggler-icon"></span>
      </button>

      <div class="collapse navbar-collapse" id="navbarNav">
        <ul class="navbar-nav ms-auto align-items-lg-center gap-lg-2 mt-3 mt-lg-0">
          <li class="nav-item">
            <RouterLink class="nav-link px-lg-3 fw-medium" to="/">Home</RouterLink>
          </li>

          <li v-if='!userStore.isLoggedIn' class="nav-item">
            <RouterLink class="nav-link px-lg-3 fw-medium" to="/login">Login</RouterLink>
          </li>

          <li v-if='!userStore.isLoggedIn' class="nav-item">
            <RouterLink class="btn btn-outline-light btn-sm rounded-pill px-4 ms-lg-2 mt-2 mt-lg-0" to="/register">
              Register
            </RouterLink>
          </li>

          <li v-if='userStore.isLoggedIn' class="nav-item">
            <RouterLink class="nav-link px-lg-3 fw-medium" to="/profile">Profile</RouterLink>
          </li>

          <li v-if='userStore.isLoggedIn' class="nav-item">
            <button class="btn btn-outline-light btn-sm rounded-pill px-4 ms-lg-2 mt-2 mt-lg-0" @click="handleLogout">
              Logout
            </button>
          </li>
        </ul>
      </div>
    </div>
  </nav>
</template>
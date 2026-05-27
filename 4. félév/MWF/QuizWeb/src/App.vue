<script setup lang="ts">
import NavBar from './components/NavBar.vue'
import { useUserStore } from './stores/userStore'
import { onMounted } from 'vue';
import { usePlayerStore } from './stores/playerStore';

const userStore = useUserStore()
const playerStore = usePlayerStore();

onMounted( () => {
  userStore.initialize();
  userStore.scheduleRefresh();
  playerStore.loadFromLocalStorage();
})

</script>

<template>
  <NavBar/>
  <router-view/>
</template>
